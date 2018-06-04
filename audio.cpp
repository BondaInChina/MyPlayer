#include "audio.h"
#include <QAudioFormat>
#include <QAudioOutput>

Audio::Audio()
{

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

void Audio::run()
{
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
    AVFrame *aFrame = av_frame_alloc();
    static AVPacket pkt;

    SwrContext *swrCtx = swr_alloc();
    swrCtx = swr_alloc_set_opts(swrCtx,
                                av_get_default_channel_layout(2),
                                AV_SAMPLE_FMT_S16,
                                mCodec->sample_rate,
                                av_get_default_channel_layout(mCodec->channels),
                                mCodec->sample_fmt,
                                mCodec->sample_rate,
                                0,
                                NULL);
    int ret = swr_init(swrCtx);

    uint8_t *pcm = NULL;

    while(1)
    {
        packet_queue_get(mQueue, &pkt, 1);
        int got_frame = 0;
        avcodec_decode_audio4(mCodec, aFrame, &got_frame, &pkt);
        if(got_frame != 0)
        {
            while (out->bytesFree() < size)
            {
                QThread::msleep(1);
            }
            if(pcm == NULL)
            {
                pcm = new uint8_t[aFrame->nb_samples * aFrame->channels * 2];
            }

            ret = swr_convert(swrCtx, &pcm, aFrame->nb_samples, (const uint8_t**)aFrame->data, aFrame->nb_samples);
            WritePcm2File("E:\\test1.pcm", pcm, aFrame->nb_samples * aFrame->channels * 2);
            io->write((const char *)pcm, aFrame->nb_samples * aFrame->channels * 2);
        }
    }
}
