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


#include <au_encode.h>
#include <QDebug>

au_encode::au_encode()
{
    got_frame=0;
    ret=0;
    size=0;

}

au_encode::~au_encode()
{

}

int au_encode::flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index)
{
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
        CODEC_CAP_DELAY))
        return 0;
    while (1) {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_audio2 (fmt_ctx->streams[stream_index]->codec, &enc_pkt,
            NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame){
            ret=0;
            break;
        }
        printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n",enc_pkt.size);
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}


int au_encode::encode_open()
{
//    in_file= fopen("tdjm.pcm", "rb");

    avcodec_register_all();

    //Method 1.
//    pFormatCtx = avformat_alloc_context();
//    fmt = av_guess_format(NULL, out_file, NULL);
//    pFormatCtx->oformat = fmt;


    //Method 2.
//    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
//    fmt = pFormatCtx->oformat;

//    Open output URL
//    if (avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE) < 0){
//        printf("Failed to open output file!\n");
//        return -1;
//    }

//    audio_st = avformat_new_stream(pFormatCtx, 0);
//    if (audio_st==NULL){
//        return -1;
//    }

    pCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!pCodec)
    {
        qDebug()<<"--- AAC codec not found\n";
        return -1;
    }

//    pCodecCtx = audio_st->codec;
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx)
    {
        qDebug()<<"--- Could not allocate audio codec context\n";
        return -1;
    }
//    pCodecCtx->codec_id = fmt->audio_codec;

    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    pCodecCtx->sample_rate= SAMPLE_RATE;
    pCodecCtx->channel_layout=AV_CH_LAYOUT_STEREO;
    pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
    pCodecCtx->bit_rate = 64000;


    //Show some information
//    av_dump_format(pFormatCtx, 0, out_file, 1);

//    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
//    if (!pCodec){
//        qDebug()<<"Can not find encoder!\n";
//        return -1;
//    }
    if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0){
        qDebug()<<"Failed to open encoder!\n";
        return -1;
    }
    pFrame = av_frame_alloc();
    pFrame->nb_samples= pCodecCtx->frame_size;
    pFrame->format= pCodecCtx->sample_fmt;


    size = av_samples_get_buffer_size(NULL, pCodecCtx->channels,pCodecCtx->frame_size,pCodecCtx->sample_fmt, 1);
    frame_buf = (uint8_t *)av_malloc(size);
    avcodec_fill_audio_frame(pFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,(const uint8_t*)frame_buf, size, 1);


    auin_fifo = av_audio_fifo_alloc(pCodecCtx->sample_fmt,pCodecCtx->channels,pCodecCtx->frame_size*10);


    av_init_packet(&pkt);

    return 0;

}

int au_encode::encode_do(void **pobuf,
                         int *polen)
{

    out = fopen("test.aac","a");

    *polen = 0;
    //Write Header
//    avformat_write_header(pFormatCtx,NULL);

    av_free_packet(&pkt);
    av_init_packet(&pkt);

        //Read PCM
//        pkt.data = frame_buf;
//        pkt.size = fread(frame_buf, 1, size, in);

        av_audio_fifo_read(auin_fifo,(void**)&frame_buf,pFrame->nb_samples);
//        if (av_audio_fifo_size(auin_fifo) == 0)
//        {
//            qDebug()<<"no more data";
//            break;
//        }

//        }
//    else if(feof(in_file)){
//            break;
//        }
//        pFrame->data[0] = frame_buf;  //PCM Data

//        pFrame->pts=i*100;
        got_frame=0;
        //Encode
        ret = avcodec_encode_audio2(pCodecCtx, &pkt,pFrame, &got_frame);
        if(ret < 0){
            qDebug()<<"Failed to encode!\n";
            return -1;
        }
        if (got_frame==1){
//            printf("Succeed to encode 1 frame! \tsize:%5d\n",pkt.size);
//            pkt.stream_index = audio_st->index;
            fwrite(pkt.data,pkt.size,1,out);
//            ret = av_write_frame(pFormatCtx, &pkt);
            *pobuf = pkt.data;
            *polen = pkt.size;

        }


    //Flush Encoder
//    ret = flush_encoder(pFormatCtx,0);
//    if (ret < 0) {
//        printf("Flushing encoder failed\n");
//        return -1;
//    }

    //Write Trailer
//    av_write_trailer(pFormatCtx);

//    fclose(in_file);

    fclose(out);

    return 0;

}

void au_encode::encode_close()
{
//    Clean
//    if (audio_st){
//        avcodec_close(audio_st->codec);
        av_free(pFrame);
        av_free(frame_buf);
//    }
//    avio_close(pFormatCtx->pb);
//    avformat_free_context(pFormatCtx);


}


