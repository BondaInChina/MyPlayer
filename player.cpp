#include "player.h"
#include <QAudioFormat>
#include <QAudioOutput>


static double avio_r2d(AVRational ration)
{
    return ration.den == 0? 0 : (double)ration.num / (double)ration.den;
}

static bool WritePcm2File(const char *path, uint8_t *data, int size)
{
    FILE *file = fopen(path, "ab+");
    if(file == NULL)
    {
        return false;
    }

    int len = fwrite(data, 1, size, file);
    if(len < size)
    {
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

Player::Player()
{

}

bool Player::Open()
{
//    QString strPath = QFileDialog::getOpenFileName(this, tr("Open file dialog"), tr("/home"), tr("Videos (*.mp4 *.avi *.mkv)"));
//    if(strPath == NULL)
//    {
//        emit sigSendErrorMsg("Open file failed!");
//        return false;
//    }
//    std::string str = strPath.toStdString();
//    mPath = str.c_str(); // 此处应该会出问题
    return true;
}

bool Player::Init(const char *path)
{
    mPath = path;
    //初始化封装库
    av_register_all();
    //解封装上下文
    mFormatCtx = NULL;
    int ret = avformat_open_input(
        &mFormatCtx,
        mPath,
        NULL,  // 0表示自动选择解封器
        NULL //参数设置，比如rtsp的延时时间
    );
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        emit sigSendErrorMsg("Open file failed!");
        return false;
    }

    avformat_find_stream_info(mFormatCtx, NULL);

    int videoIndex = -1;
    int audioIndex = -1;
    for(int i = 0; i < mFormatCtx->nb_streams; i++)
    {
        if(mFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoIndex = i;
        }
        else if(mFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioIndex = i;
        }
    }
    mVideoStream = mFormatCtx->streams[videoIndex];
    mAudioStream = mFormatCtx->streams[audioIndex];

    // 找视频解码器
    mVideoCodecCtx= mVideoStream->codec;
    mVideoCodec = avcodec_find_decoder(mVideoCodecCtx->codec_id);
    if(mVideoCodec == NULL)
    {
        emit sigSendErrorMsg("Can not find video decoder!");
        return false;
    }
    ret = avcodec_open2(mVideoCodecCtx, mVideoCodec, NULL);
    if(ret != 0)
    {
        emit sigSendErrorMsg("Can not open video decoder!");
        return false;
    }

    // 找音频解码器
    mAudioCodecCtx= mAudioStream->codec;
    mAudioCodec = avcodec_find_decoder(mAudioCodecCtx->codec_id);
    if(!mAudioCodec)
    {
        emit sigSendErrorMsg("Can not find audio decoder!");
        return false;
    }
    ret = avcodec_open2(mAudioCodecCtx, mAudioCodec, NULL);
    if(ret != 0)
    {
        emit sigSendErrorMsg("Can not open audio decoder!");
        return false;
    }
}

void Player::run()
{
//    int64_t time = 200;
//    av_seek_frame(ic, videoIndex,
//                  (double)time / (double)avio_r2d(mVideoStream->time_base),
//                  AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);

    QAudioFormat fmt;
    fmt.setSampleRate(48000);
    fmt.setSampleSize(16);
    fmt.setChannelCount(2);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);
    QAudioOutput *out = new QAudioOutput(fmt);
    QIODevice *io = out->start(); //开始播放音频
    int size = out->periodSize();
    char *buf = new char[size];

    int got_frame = 0;
    AVFrame *vFrame = av_frame_alloc();
    AVFrame *aFrame = av_frame_alloc();
    AVPacket *pkt = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(pkt, mVideoCodecCtx->width * mVideoCodecCtx->height);
    //av_init_packet(pkt);
    SwsContext *sws = sws_getContext(mVideoStream->codec->width,
                                     mVideoStream->codec->height,
                                     mVideoStream->codec->pix_fmt,
                                     mVideoStream->codec->width,
                                     mVideoStream->codec->height,
                                     AV_PIX_FMT_RGB32,
                                     SWS_BICUBIC,
                                     NULL,
                                     NULL,
                                     NULL
                                     );
    uint8_t* rgb[2] = {0};
    int index = 0;

    SwrContext *swrCtx = swr_alloc();
    swrCtx = swr_alloc_set_opts(swrCtx,
                                av_get_default_channel_layout(2),
                                AV_SAMPLE_FMT_S16,
                                mAudioCodecCtx->sample_rate,
                                av_get_default_channel_layout(mAudioCodecCtx->channels),
                                mAudioCodecCtx->sample_fmt,
                                mAudioCodecCtx->sample_rate,
                                0,
                                NULL);
    int ret = swr_init(swrCtx);

    uint8_t *pcm = NULL;


    while(1)
    {
        if(av_read_frame(mFormatCtx,pkt) != 0)
        {
            break;
        }
        if(pkt->stream_index == mVideoStream->index)
        {
            avcodec_decode_video2(mVideoCodecCtx, vFrame, &got_frame, pkt);
            if(got_frame != 0)
            {
                if(rgb[0] == NULL)
                {
                    rgb[0] = new unsigned char[vFrame->width * vFrame->height * 4];
                }
                int lines[2] = {vFrame->width * 4};

                int ret = sws_scale(sws, vFrame->data, vFrame->linesize,0,vFrame->height,rgb,lines);

                //把这个RGB数据 用QImage加载
                QImage tmpImg(rgb[0],vFrame->width,vFrame->height,QImage::Format_RGB32);
                QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                emit sigGetOneFrame(tmpImg);  //发送信号
            }
        }
        if(pkt->stream_index == mAudioStream->index)
        {
            got_frame = 0;
            avcodec_decode_audio4(mAudioCodecCtx, aFrame, &got_frame, pkt);
            if(got_frame != 0)
            {
                while (out->bytesFree() < size)
                {
                    //QThread::msleep(1);
                }
                if(pcm == NULL)
                {
                    pcm = new uint8_t[aFrame->nb_samples * aFrame->channels * 2];
                }

                ret = swr_convert(swrCtx, &pcm, aFrame->nb_samples, (const uint8_t**)aFrame->data, aFrame->nb_samples);
                WritePcm2File("E:\\test0.pcm", pcm, aFrame->nb_samples * aFrame->channels * 2);
                io->write((const char *)pcm, aFrame->nb_samples * aFrame->channels * 2);
            }
        }
    }

    if (mFormatCtx)
    {
        avformat_close_input(&mFormatCtx);
    }
    av_free_packet(pkt);
}
