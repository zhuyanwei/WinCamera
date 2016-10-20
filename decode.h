#ifndef DECODE_H
#define DECODE_H

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif


extern "C"{
#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ffmpeg_common.h>
#endif
}

#include "comdef.h"
#include <QObject>
#include <assert.h>

#include <highgui.h>  //包含opencv库头文件
#include <cv.h>

#define  WIDTH 320
#define  HEIGHT 240

class decode : public QObject
{
    Q_OBJECT
public:
    decode();
    ~decode();

    int decode_open();
    int decode_do(void *inbuf_ptr, int size, void **showbuf);
    void decode_close();
    int convert_open();

    void IplImage_to_AVFrame(IplImage* iplImage, AVFrame* avFrame, int frameWidth, int frameHeight) ;
    IplImage *AVFrame_to_IplImage(AVFrame *src, IplImage *dst,int width,int height);

//    IplImage *imgBgr;

    AVCodec *codec;
    AVCodecContext *c ;
//    AVFormatContext *f;
    int  got_picture, len;
    AVFrame *picture;
    AVPacket packet;

    struct SwsContext * swscontext;

    int dst_size;
    uint8_t *dst_buffer;
    AVFrame *dst_frame;



private:

signals:

public slots:
};

#endif // DECODE_H
