#ifndef COMMON_H
#define COMMON_H
#include <thread>
#include <condition_variable>
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
using namespace std;


typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    mutex *pMutex;
    condition_variable *pCondition;
} PacketQueue;

int InitQueue(PacketQueue *queue);
int packet_queue_put(PacketQueue *q, AVPacket *pkt);
int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block);


#endif // COMMON_H
