#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QThread>
#include "common.h"

class Audio : public QThread
{
public:
    Audio();
    PacketQueue *mQueue;
    AVCodecContext *mCodec;
    void SetAudioStream(AVStream *stream);
    AVStream *mAudioStream;
protected:
    void run();
};

#endif // AUDIO_H
