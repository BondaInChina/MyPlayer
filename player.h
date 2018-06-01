#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <iostream>
using namespace std;

#ifndef _WIN32
static const char* mp4File = "/home/test.mp4";
static const char* yuvFile = "/home/liangjf/yuv.yuv";
static const char* rgbFile = "/home/liangjf/rgb.yuv";
static const char* jpegFile = "/home/liangjf";
extern "C"{
#include "/usr/local/ffmpeg/include/libavformat/avformat.h"
#include "/usr/local/ffmpeg/include/libavcodec/avcodec.h"
#include "/usr/local/ffmpeg/include/libswscale/swscale.h"
}
#else
static const char* mp4File = "E:\\test.mp4";
static const char* yuvFile = "E:\\yuv.yuv";
static const char* rgbFile = "E:\\rgb.yuv";
static const char* jpegFile = "E:\\";
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
#endif

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
signals:
    void sigGetOneFrame(QImage);
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
