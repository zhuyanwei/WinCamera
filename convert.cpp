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


#include <convert.h>
#include <QDebug>

convert::convert()
{

}

convert::~convert()
{

}
//enum convert::AVPixelFormat v4lFmt2AVFmt(U32 v4lfmt)
//{
//	enum AVPixelFormat avfmt = AV_PIX_FMT_NONE;

//	switch (v4lfmt)
//	{
//		case V4L2_PIX_FMT_YUV420:
//			avfmt = AV_PIX_FMT_YUV420P;
//			break;
//		case V4L2_PIX_FMT_YUYV:
//			avfmt = AV_PIX_FMT_YUYV422;
//			break;
//		case V4L2_PIX_FMT_RGB565:
//			avfmt = AV_PIX_FMT_RGB565LE;
//			break;
//		case V4L2_PIX_FMT_RGB24:
//			avfmt = AV_PIX_FMT_RGB24;
//			break;
//		default:
//			printf("!!! Unsupported v4l2 format: %d\n", v4lfmt);
//			break;
//	}

//	return avfmt;
//}

int convert::convert_open(int inwidth,int inheight,AVPixelFormat inpixfmt,int outwidth,int outheight,AVPixelFormat outpixfmt)
{

    this->sws_ctx = NULL;
    this->src_buffer = NULL;
    this->src_buffersize = 0;
    this->src_frame = NULL;
    this->dst_buffer = NULL;
    this->dst_buffersize = 0;
    this->dst_frame = NULL;
    this->inavfmt = AV_PIX_FMT_NONE;
    this->outavfmt = AV_PIX_FMT_NONE;
    this->inwidth = inwidth;
    this->inheight = inheight;
//    this->inpixfmt = inpixfmt;
    this->inavfmt = inpixfmt;
    this->outwidth = outwidth;
    this->outheight = outheight;
//    this->outpixfmt = outpixfmt;
    this->outavfmt = outpixfmt;

    this->sws_ctx = sws_getContext(this->inwidth,
            this->inheight, this->inavfmt, this->outwidth,
            this->outheight, this->outavfmt, SWS_BILINEAR, NULL,
			NULL, NULL);
    if (!this->sws_ctx)
	{
		printf("--- Create scale context failed\n");
		goto err0;
	}

	// alloc buffers
    this->src_frame = av_frame_alloc();
    if (!this->src_frame)
	{
		printf("--- allocate src_frame failed\n");
		goto err1;
	}
    this->src_buffersize = avpicture_get_size(this->inavfmt,
            this->inwidth, this->inheight);
    this->src_buffer = (uint8_t *) av_malloc(this->src_buffersize);
    if (!this->src_buffer)
	{
        printf("--- allocate src_buffer failed\n");
		goto err2;
	}
    avpicture_fill((AVPicture *) this->src_frame, this->src_buffer,
            this->inavfmt, this->inwidth, this->inheight);

    this->dst_frame = av_frame_alloc();
    if (!this->dst_frame)
	{
		printf("--- allocate dst_frame failed\n");
		goto err3;
	}
    this->dst_buffersize = avpicture_get_size(this->outavfmt,
            this->outwidth, this->outheight);
    this->dst_buffer = (uint8_t *) av_malloc(this->dst_buffersize);
    if (!this->dst_buffer)
	{
		printf("--- allocate dst_buffer failed\n");
		goto err4;
	}
    avpicture_fill((AVPicture *) this->dst_frame, this->dst_buffer,
            this->outavfmt, this->outwidth,
            this->outheight);

	printf("+++ Convert Opened\n");
    return 0;

    err4: av_frame_free(&this->dst_frame);
    err3: av_free(this->src_buffer);
    err2: av_frame_free(&this->src_frame);
    err1: sws_freeContext(this->sws_ctx);
    err0: free(this);
    return -1;
}

void convert::convert_close()
{
    av_free(this->dst_buffer);
    av_frame_free(&this->dst_frame);
    av_free(this->src_buffer);
    av_frame_free(&this->src_frame);
    sws_freeContext(this->sws_ctx);
    free(this);
	printf("+++ Convert Closed\n");
}

int convert::convert_do(void *inbuf, int isize,
		void **poutbuf, int *posize)
{
    if(this->inavfmt == this->outavfmt)
    {
        *poutbuf = inbuf;
        *posize = isize;
        return 0;
    }

    assert(isize == this->src_buffersize);
    memcpy(this->src_buffer, inbuf, isize);
    sws_scale(this->sws_ctx,
            (const uint8_t * const *) this->src_frame->data,
            this->src_frame->linesize, 0, this->inheight,
            this->dst_frame->data, this->dst_frame->linesize);
    *poutbuf = this->dst_buffer;
    *posize = this->dst_buffersize;

	return 0;
}
