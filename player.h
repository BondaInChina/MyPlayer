#ifndef PLAYER_H
#define PLAYER_H


#include <QObject>
#include <QThread>
#include <QImage>
#include <iostream>
#include "video.h"
#include "audio.h"
using namespace std;



class Player : public QThread
{
    Q_OBJECT
public:
    explicit Player();
    bool Open(); // 打开文件
    bool Close(); // 关闭正在播放的视频文件
//    bool Start(); // 开始播放
//    bool Pause(); // 暂停
//    bool Seek();  // 拖动
//    bool Stop(); // 停止播放
    bool Init(const char *path);
    Video video;
    Audio audio;
signals:
    void sigSendErrorMsg(QString);
protected:
    void run();
private:
    const char *mPath; // 视频文件路径
    AVFormatContext *mFormatCtx;
    AVStream *mVideoStream;
    AVStream *mAudioStream;
    AVCodec *mVideoCodec;
    AVCodec *mAudioCodec;
    AVCodecContext *mVideoCodecCtx;
    AVCodecContext *mAudioCodecCtx;


};

#endif // PLAYER_H
