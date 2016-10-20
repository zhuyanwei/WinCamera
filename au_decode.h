#ifndef AU_DECODE_H
#define AU_DECODE_H

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000


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
#include <stdlib.h>
#include <ffmpeg_common.h>
#endif

}

#include "comdef.h"
#include <QObject>
#include <assert.h>



class au_decode
{
public:
    au_decode();
    ~au_decode();

    AVFormatContext* pFormatCtx;
    AVOutputFormat* fmt;
    AVStream* audio_st;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;

    AVAudioFifo* auout_fifo;


    int16_t* frame_buf;
    uint8_t* out_buf;
    AVFrame* pFrame;
    AVPacket pkt;

    FILE* pcm;

    int got_frame;
    int ret;
    int size,out_size;

    int decode_open();

    void decode_close();

    int decode_do(void *ibuf,
            int ilen);

    int AudioResampling(AVCodecContext * audio_dec_ctx, AVFrame * pAudioDecodeFrame,
                        int out_sample_fmt, int out_channels , int out_sample_rate , uint8_t * out_buf);


};

#endif // AU_DECODE_H
