#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QThread>
#include <QImage>
#include "common.h"

class Video : public QThread
{
    Q_OBJECT
public:
    Video();
    PacketQueue *mQueue;
    AVCodecContext *mCodec;
    AVStream *mVideoStream;
    void SetVideoStream(AVStream *stream);
signals:
    void sigGetOneFrame(QImage);
protected:
    void run();
};

#endif // VIDEO_H
