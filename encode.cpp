/*
 * Copyright (c) 2014 Andy Huang <andyspider@126.com>
 *
 * This file is part of Camkit.
 *
 * Camkit is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Camkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Camkit; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <encode.h>
#include <QDebug>

encode::encode()
{

}

encode::~encode()
{

}

int encode::encode_open(int src_picwidth,int src_picheight,int enc_picwidth, int enc_picheight,
                        int fps,int bitrate,int gop, int chroma_interleave)
{


    this->codec = NULL;
    this->ctx = NULL;
    this->frame = NULL;
    this->inbuffer = NULL;
    this->inbufsize = 0;
    this->frame_counter = 0;
    this->src_picwidth = src_picwidth;
    this->src_picheight = src_picheight;
    this->enc_picwidth = enc_picwidth;
    this->enc_picheight = enc_picheight;
    this->fps = fps;
    this->bitrate = bitrate;
    this->gop = gop;
    this->chroma_interleave = chroma_interleave;

	avcodec_register_all();
    this->codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!this->codec)
	{
        qDebug()<<"--- H264 codec not found\n";
		goto err0;
	}

    this->ctx = avcodec_alloc_context3(this->codec);
    if (!this->ctx)
	{
		printf("--- Could not allocate video codec context\n");
		goto err0;
	}
    this->ctx->bit_rate = this->bitrate * 1000;    // to kbps
    this->ctx->width = this->src_picwidth;
    this->ctx->height = this->src_picheight;
    this->ctx->time_base = (AVRational
			)
            { 1, this->fps };    // frames per second
    this->ctx->gop_size = this->gop;
    this->ctx->max_b_frames = 1;
    this->ctx->pix_fmt = AV_PIX_FMT_YUV420P;
//	this->ctx->thread_count = 1;
	// eliminate frame delay!
    av_opt_set(this->ctx->priv_data, "preset", "slow", 0);
//    av_opt_set(this->ctx->priv_data, "tune", "zerolatency", 0);
//    av_opt_set(this->ctx->priv_data, "x264opts",
//            "no-mbtree:sliced-threads:sync-lookahead=0", 0);

    if (avcodec_open2(this->ctx, this->codec, NULL) < 0)
	{
		printf("--- Could not open codec\n");
		goto err1;
	}

    this->frame = av_frame_alloc();
    if (!this->frame)
	{
		printf("--- Could not allocate video frame\n");
		goto err2;
	}

    this->frame->format = this->ctx->pix_fmt;
    this->frame->width = this->ctx->width;
    this->frame->height = this->ctx->height;
    this->inbufsize = avpicture_get_size(AV_PIX_FMT_YUV420P,
            this->src_picwidth, this->src_picheight);
    this->inbuffer = (uint8_t*)av_malloc(this->inbufsize);
    if (!this->inbuffer)
	{
		printf("--- Could not allocate inbuffer\n");
		goto err3;
	}
    avpicture_fill((AVPicture *) this->frame, this->inbuffer,
            AV_PIX_FMT_YUV420P, this->src_picwidth,
            this->src_picheight);

    av_init_packet(&this->packet);
    this->packet.data = NULL;
    this->packet.size = 0;

	printf("+++ Encode Opened\n");
    return 0;

    err3: av_frame_free(&this->frame);
    err2: avcodec_close(this->ctx);
    err1: av_free(this->ctx);
    err0: free(this);
    return -1;
}

void encode::encode_close()
{
    av_free_packet(&this->packet);
    av_free(this->inbuffer);
    av_frame_free(&this->frame);
    avcodec_close(this->ctx);
    av_free(this->ctx);
    free(this);
	printf("+++ Encode Closed\n");
}

int encode::encode_do(void *ibuf, int ilen, void **pobuf,
		int *polen, enum pic_t *type)
{
	int got_output, ret;
	// reinit pkt
    av_free_packet(&this->packet);
    av_init_packet(&this->packet);
    this->packet.data = NULL;
    this->packet.size = 0;

    assert(this->inbufsize == ilen);
    memcpy(this->inbuffer, ibuf, ilen);
    this->frame->pts = this->frame_counter++;

    ret = avcodec_encode_video2(this->ctx, &this->packet, this->frame,
			&got_output);
	// cancel key frame
    this->frame->pict_type =(AVPictureType)0;
    this->frame->key_frame = 0;
	if (ret < 0)
	{
		printf("--- Error encoding frame\n");
		return -1;
	}

	if (got_output)
	{
        *pobuf = this->packet.data;
        *polen = this->packet.size;
        switch (this->ctx->coded_frame->pict_type)
		{
			case AV_PICTURE_TYPE_I:
				*type = I;
				break;
			case AV_PICTURE_TYPE_P:
				*type = P;
				break;
			case AV_PICTURE_TYPE_B:
				*type = B;
				break;
			default:
				*type = NONE;
				break;
		}
	}
	else	// get the delayed frame
	{
		printf("!!! encoded frame delayed!\n");
		*pobuf = NULL;
		*polen = 0;
		*type = NONE;
	}

	return 0;
}

int encode::encode_get_headers(void **pbuf, int *plen,
		enum pic_t *type)
{
	*pbuf = NULL;
	*plen = 0;
	*type = NONE;

	return 0;
}

int encode::encode_set_qp(int val)
{
    UNUSED(this);
	UNUSED(val);
	printf("*** %s.%s: This function is not implemented\n", __FILE__,
			__FUNCTION__);
	return -1;
}

int encode::encode_set_bitrate(int val)
{
    this->ctx->bit_rate = val;
	return 0;
}

int encode::encode_set_framerate(int val)
{
    this->ctx->time_base = (AVRational
			)
			{ 1, val };
	return 0;
}

void encode::encode_force_Ipic()
{
    this->frame->pict_type = AV_PICTURE_TYPE_I;
    this->frame->key_frame = 1;

	return;
}
