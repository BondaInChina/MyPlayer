#include "video.h"

Video::Video()
{

}

void Video::run()
{
    SwsContext *sws = sws_getContext(mCodec->width,
                                     mCodec->height,
                                     mCodec->pix_fmt,
                                     mCodec->width,
                                     mCodec->height,
                                     AV_PIX_FMT_RGB32,
                                     SWS_BICUBIC,
                                     NULL,
                                     NULL,
                                     NULL
                                     );
    uint8_t* rgb[2] = {0};
    int index = 0;
    static AVPacket pkt;
    int got_frame = 0;
    AVFrame *vFrame = av_frame_alloc();
    while(1)
    {
        packet_queue_get(mQueue, &pkt, 0);
        avcodec_decode_video2(mCodec, vFrame, &got_frame, &pkt);
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


}
