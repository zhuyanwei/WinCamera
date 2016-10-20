#include "decode.h"
#include <QDebug>

decode::decode()
{

}

decode::~decode()
{

}

int decode::decode_open()
{
//    avcodec_init();
    avcodec_register_all();
    /* find the mpeg1 video decoder */
    codec = avcodec_find_decoder(CODEC_ID_H264);
    if (!codec)
    {
        fprintf(stderr, "codec not found\n");
    }

//    c= avcodec_alloc_context();
//    picture= avcodec_alloc_frame();

    c = avcodec_alloc_context3(codec);
    picture = av_frame_alloc();

    if(codec->capabilities&CODEC_CAP_TRUNCATED)
    {
        c->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */
    }
        c->width = WIDTH;
        c->height = HEIGHT;
        c->bit_rate = 400000;
        c->time_base.den = FRAMERATE;
        c->time_base.num = 1;
        c->gop_size =10;
        c->max_b_frames =1;
        c->pix_fmt = AV_PIX_FMT_YUV420P;

    av_opt_set(c->priv_data, "preset", "slow", 0);

    c->extradata = new uint8_t[35];//给extradata成员参数分配内存
    c->extradata_size = 35;//extradata成员参数分配内存大小

   //给extradata成员参数设置值
    //00 00 00 01
    c->extradata[0] = 0x00;
    c->extradata[1] = 0x00;
    c->extradata[2] = 0x00;
    c->extradata[3] = 0x01;
    //67 42 80 1e
    c->extradata[4] = 0x67;
    c->extradata[5] = 0x64;
    c->extradata[6] = 0x00;
    c->extradata[7] = 0x0;
    //88 8b 40 50
    c->extradata[8] = 0xac;
    c->extradata[9] = 0xec;
    c->extradata[10] = 0x14;
    c->extradata[11] = 0x1f;
    //1e d0 80 00
    c->extradata[12] = 0xa1;
    c->extradata[13] = 0x00;
    c->extradata[14] = 0x00;
    c->extradata[15] = 0x03;
    //03 84 00 00
    c->extradata[16] = 0x00;
    c->extradata[17] = 0x01;
    c->extradata[18] = 0x00;
    c->extradata[19] = 0x00;
    //af c8 02 00
    c->extradata[20] = 0x03;
    c->extradata[21] = 0x00;
    c->extradata[22] = 0x1e;
    c->extradata[23] = 0x8f;
    //00 00 00 01
    c->extradata[24] = 0x14;
    c->extradata[25] = 0x29;
    c->extradata[26] = 0x38;
    c->extradata[27] = 0x00;
    //68 ce 38 80
    c->extradata[28] = 0x00;
    c->extradata[29] = 0x00;
    c->extradata[30] = 0x01;
    c->extradata[31] = 0x68;
    c->extradata[32] = 0xef;
    c->extradata[33] = 0xbc;
    c->extradata[34] = 0xb0;

    /* for some codecs, such as msmpeg4 and mpeg4, width and height
    MUST be initialized there because these info are not available
    in the bitstream */

    /* open it */
//    if (avcodec_open(c, codec) < 0)
    if(avcodec_open2(c,codec,NULL)<0)
    {
        fprintf(stderr, "could not open codec\n");
    }
//    fout = fopen(DecodeOutputFileName_Vedio, "wb");
//    if (!fout)
//    {
//        fprintf(stderr, "could not open %s\n", DecodeOutputFileName_Vedio);
//    }

    av_init_packet(&this->packet);

    this->packet.data = NULL;
    this->packet.size = 0;


    return 1;
}

int decode::convert_open()
{
    AVPixelFormat dst_fmt = AV_PIX_FMT_BGR24;
    swscontext = sws_getContext(WIDTH, HEIGHT, AV_PIX_FMT_YUV420P, WIDTH, HEIGHT,dst_fmt,SWS_BICUBIC, 0, 0, 0);
    if (swscontext == 0)
    {
        return(0);
    }

    dst_frame = av_frame_alloc();
    dst_size = avpicture_get_size(dst_fmt,WIDTH, HEIGHT);
    dst_buffer = (uint8_t *) av_malloc(dst_size);
    avpicture_fill((AVPicture*)dst_frame, dst_buffer, dst_fmt,  WIDTH, HEIGHT);

    return 1;
}



int decode::decode_do(void *inbuf_ptr, int size, void **showbuf)
{
//    IplImage * dst = 0;

    av_free_packet(&this->packet);
    av_init_packet(&this->packet);
    this->packet.data = (uchar *)inbuf_ptr;
    this->packet.size = size;

    len = avcodec_decode_video2(c, picture,
            &got_picture,&this->packet);

//    len = avcodec_decode_video(c, picture, &got_picture,
//        inbuf_ptr, size);
    if (len < 0)
    {
//        qDebug()<<"Error while decoding frame %d\n";
    }

//    qDebug()<<"got_picture"<<got_picture;
    if (got_picture)
    {
//        fflush(stdout);
        /* the picture is allocated by the decoder. no need to
        free it */
        //pgm_save(picture->data[0], picture->linesize[0], //Y
        //	c->width, c->height, DecodeOutputFileName_Vedio);
        //pgm_save(picture->data[1], picture->linesize[1],
        //	c->width/2, c->height/2, DecodeOutputFileName_Vedio);       //U
        //pgm_save(picture->data[2], picture->linesize[2],
        //	c->width/2, c->height/2, DecodeOutputFileName_Vedio);       //V
//        *showbuf = AVFrame_to_IplImage(picture,dst,c->width,c->height);

        //    CvvImage m_CvvImage;
//            imgBgr  = cvCreateImage(cvSize(c->width, c->height), IPL_DEPTH_8U, 3);
        //    CRect rect;
        //    ::GetClientRect(m_hWnd, &rect);
        //    HDC  hdc = ::GetDC(m_hWnd);
            sws_scale(swscontext, picture->data, picture->linesize, 0, c->height, dst_frame->data, dst_frame->linesize);

//            imgBgr->imageData = (char*)bgr->data[0];
            *showbuf=dst_buffer;

            return 1;


        //    m_CvvImage.CopyOf(imgBgr,1); //复制该帧图像
        //    m_CvvImage.DrawToHDC(hdc, &rect);              //显示到设备的矩形框内

//            if (buf)
//            {
//                free(buf);
//                buf = NULL;
//            }
//            if (bgr)
//            {
//                av_free(bgr);
//                bgr = NULL;
//            }
//            if (swscontext)
//            {
//                sws_freeContext(swscontext);
//                swscontext = NULL;
//            }
//                cvReleaseImage(&imgBgr);
//                if (imgBgr)
//                {
//                    imgBgr = NULL;
//                }

   }

    return 0;
}

void decode::decode_close()
{
    av_free(dst_buffer);
    av_frame_free(&dst_frame);
    sws_freeContext(swscontext);

    av_free_packet(&this->packet);
    av_frame_free(&this->picture);
    avcodec_close(this->c);
    av_free(this->c);
    free(this);
    printf("+++ Decode Closed\n");
}

void decode::IplImage_to_AVFrame(IplImage* iplImage, AVFrame* avFrame, int frameWidth, int frameHeight)
{
    struct SwsContext * img_convert_ctx = 0;
    int linesize[4] = {0, 0, 0, 0};
    img_convert_ctx = sws_getContext(iplImage->width, iplImage->height,PIX_FMT_BGR24,frameWidth,frameHeight,PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
    if (img_convert_ctx != 0)
    {
        linesize[0] = 3 * iplImage->width;
        sws_scale(img_convert_ctx, (uint8_t**)iplImage->imageData, linesize, 0, iplImage->height, avFrame->data, avFrame->linesize);
        sws_freeContext(img_convert_ctx);
    }
}

//IplImage *decode::AVFrame_to_IplImage(AVFrame *src, IplImage *dst,int width,int height)
//{
//    cvReleaseImage(&imgBgr);
//    if (imgBgr)
//    {
//        imgBgr = NULL;
//    }
////    CvvImage m_CvvImage;
//    imgBgr  = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
////    CRect rect;
////    ::GetClientRect(m_hWnd, &rect);
////    HDC  hdc = ::GetDC(m_hWnd);

//    struct SwsContext * swscontext = sws_getContext(width, height, PIX_FMT_YUV420P, width, height, PIX_FMT_BGR24,SWS_BICUBIC, 0, 0, 0);
//    if (swscontext == 0)
//    {
//        return(0);
//    }

//    AVFrame * bgr = av_frame_alloc();
//    uint8_t *buf = (uint8_t*)malloc(avpicture_get_size(PIX_FMT_BGR24, width, height));
//    avpicture_fill((AVPicture*)bgr, buf, PIX_FMT_BGR24,  width, height);
//    sws_scale(swscontext, src->data, src->linesize, 0, height, bgr->data, bgr->linesize);

//    imgBgr->imageData = (char*)bgr->data[0];


////    m_CvvImage.CopyOf(imgBgr,1); //复制该帧图像
////    m_CvvImage.DrawToHDC(hdc, &rect);              //显示到设备的矩形框内

//    if (buf)
//    {
//        free(buf);
//        buf = NULL;
//    }
//    if (bgr)
//    {
//        av_free(bgr);
//        bgr = NULL;
//    }
//    if (swscontext)
//    {
//        sws_freeContext(swscontext);
//        swscontext = NULL;
//    }

//    return imgBgr;
//}
