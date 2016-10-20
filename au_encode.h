#ifndef AU_ENCODE_H
#define AU_ENCODE_H

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


class au_encode : public QObject
{
    Q_OBJECT
public:
    au_encode();
    ~au_encode();


    AVFormatContext* pFormatCtx;
    AVOutputFormat* fmt;
    AVStream* audio_st;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;

    AVAudioFifo* auin_fifo;

    uint8_t* frame_buf;
    AVFrame* pFrame;
    AVPacket pkt;

    int got_frame;
    int ret;
    int size;

//    FILE *in_file=NULL;                         //Raw PCM data
//    int framenum= 1000;                          //Audio frame number
//    const char* out_file = "tdjm.aac";          //Output URL
    int i;
    FILE *in;
    FILE *out;

    int encode_open();

    void encode_close();

    int encode_do(void **pobuf,
            int *polen);



private:

    int flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index);


signals:
    
public slots:
    
};

#endif // AU_ENCODE_H
