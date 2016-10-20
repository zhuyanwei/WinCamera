#ifndef CONVERT_H
#define CONVERT_H

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
#include <stdlib.h>
#include <ffmpeg_common.h>
#endif
}

#include "comdef.h"
#include <QObject>
#include <assert.h>

#include <highgui.h>  //包含opencv库头文件
#include <cv.h>

class convert : public QObject
{
    Q_OBJECT
public:
    convert();
    ~convert();

    struct SwsContext *sws_ctx;
    uint8_t *src_buffer;
    int src_buffersize;
    AVFrame *src_frame;
    uint8_t *dst_buffer;
    int dst_buffersize;
    AVFrame *dst_frame;
    enum AVPixelFormat inavfmt;
    enum AVPixelFormat outavfmt;


    int inwidth; /**< input image width */
    int inheight; /**< input image height */
    AVPixelFormat inpixfmt; /**< input image pixel format */
    int outwidth; /**< output image width */
    int outheight; /**< output image height */
    AVPixelFormat outpixfmt; /**< output image pixel format */

    int convert_open(int inwidth, int inheight, AVPixelFormat inpixfmt, int outwidth, int outheight, AVPixelFormat outpixfmt);

    void convert_close();

    int convert_do(void *inbuf, int isize,void **poutbuf, int *posize);

private:
//    enum AVPixelFormat v4lFmt2AVFmt(U32 v4lfmt);
    void yuv422_to_yuv420(uint8_t *inbuf, uint8_t *outbuf, int width,int height);

signals:
    
public slots:
    
};

#endif // CONVERT_H


