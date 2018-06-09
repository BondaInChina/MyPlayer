#include "player.h"

Player::Player()
{

}

int64_t Player::mClock = 0;

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

bool Player::Close()
{
    if (mFormatCtx)
    {
        avformat_close_input(&mFormatCtx);
    }
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

    video.SetVideoStream(mVideoStream);
    audio.SetAudioStream(mAudioStream);
    video.mQueue = new PacketQueue();
    InitQueue(video.mQueue);
    audio.mQueue = new PacketQueue();
    InitQueue(audio.mQueue);
    video.mCodec = mVideoCodecCtx;
    audio.mCodec = mAudioCodecCtx;
    video.start();
    audio.start();

    AVPacket *pkt = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(pkt, mVideoCodecCtx->width * mVideoCodecCtx->height);
    //av_init_packet(pkt);

    while(1)
    {
        if(av_read_frame(mFormatCtx,pkt) != 0)
        {
            break;
        }
        if(pkt->stream_index == mVideoStream->index)
        {
            packet_queue_put(video.mQueue, pkt);
        }
        if(pkt->stream_index == mAudioStream->index)
        {
            packet_queue_put(audio.mQueue, pkt);
        }
    }
    av_free_packet(pkt);
}
