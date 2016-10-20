#ifndef ENCODE_H
#define ENCODE_H

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

class encode : public QObject
{
    Q_OBJECT
public:
    encode();
    ~encode();

    enum pic_t
    {
        NONE = -1, SPS, PPS, I, P, B
    };

    AVCodec *codec;
    AVCodecContext *ctx;
    AVFrame *frame;
    uint8_t *inbuffer;
    int inbufsize;
    AVPacket packet;
    unsigned long frame_counter;

    int src_picwidth; /**< the source picture width*/
    int src_picheight; /**< the source picture height */
    int enc_picwidth; /**< the encoded picture width */
    int enc_picheight; /**< the encode picture height */
    int fps; /**< frames per second */
    int bitrate; /**< bit rate (kbps), set bit rate to 0 means no rate control, the rate will depend on QP */
    int gop; /**< the size of group of pictures */
    int chroma_interleave; /**< whether chroma interleaved? */

    int encode_open(int src_picwidth,int src_picheight,int enc_picwidth, int enc_picheight,
                                   int fps,int bitrate,int gop, int chroma_interleave);

    void encode_close();

    int encode_get_headers(void **pbuf, int *plen,
            enum pic_t *type);

    int encode_do(void *ibuf, int ilen, void **pobuf,
            int *polen, enum pic_t *type);

    int encode_set_qp(int val);

    int encode_set_gop(int val);

    int encode_set_bitrate(int val);

    int encode_set_framerate( int val);

    void encode_force_Ipic();

private:


signals:
    
public slots:
    
};

#endif // ENCODE_H
