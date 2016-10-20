#include "camerathread.h"

#include<QDebug>

CameraThread::CameraThread(RTPSession *session)
{   
    cp= new cameraGet();
    cv= new convert();
    en= new encode();
    rs= new rtpsend();

    // set paraments
    AVPixelFormat vfmt = AV_PIX_FMT_BGR24;
    AVPixelFormat ofmt = AV_PIX_FMT_YUV420P;

    cv->inwidth = WIDTH;
    cv->inheight = HEIGHT;
    cv->inpixfmt = vfmt;
    cv->outwidth = WIDTH;
    cv->outheight = HEIGHT;
    cv->outpixfmt = ofmt;

    en->src_picwidth = WIDTH;
    en->src_picheight = HEIGHT;
    en->enc_picwidth = WIDTH;
    en->enc_picheight = HEIGHT;
    en->chroma_interleave = 0;
    en->fps = FRAMERATE;
    en->gop = 10;
    en->bitrate = 400;

    rs->max_pkt_len = MAXDATASIZE - 20;
    rs->ssrc = SSRC;

    pac_buf = (char *) malloc(MAXDATASIZE);

    ret = cp->opencamera(WIDTH,HEIGHT);
    if(-1==ret)
    {
        qDebug()<<"open camera fail";
        cp->closecamera();
    }

    ret = cv->convert_open(cv->inwidth,cv->inheight, cv->inpixfmt, cv->outwidth, cv->outheight, cv->outpixfmt);

    ret = en->encode_open(en->src_picwidth,en->src_picheight,en->enc_picwidth, en->enc_picheight,
                    en->fps,en->bitrate,en->gop, en->chroma_interleave);

    ret = rs->pack_open(rs->max_pkt_len,rs->ssrc);

    ret = rs->net_open(session);

    stopped = false;

}

CameraThread::~CameraThread()
{
    free(pac_buf);
    rs->net_close();
    en->encode_close();
    cv->convert_close();

    cp->closecamera();
}

void CameraThread::run()
{
    while(!stopped)
    {
    ret = cp->readFarme(&cap_frame);

    emit captured();

    cap_buf=(void *)cap_frame->imageData;
    cap_len=cap_frame->imageSize;

    ret = cv->convert_do(cap_buf, cap_len, &cvt_buf, &cvt_len);
    if (ret < 0)
    {
       qDebug()<<"--- convert_do failed";
//         break;
    }
    if (cvt_len <= 0)
    {
        qDebug()<<"!!! No convert data\n";
//        continue;
    }
    // else

        // fetch h264 headers first!
//    while ((ret = en->encode_get_headers(&hd_buf, &hd_len, &ptype))
//            == 1)
//    {
//        //fwrite(hd_buf, 1, hd_len, dumpfile);
//        pa->pack_put(hd_buf, hd_len);
//        while (pa->pack_get(pac_buf, MAX_RTP_SIZE, &pac_len) == 1)
//        {
//            ret = ne->net_send(pac_buf, pac_len);
//            if (ret != pac_len)
//            {
//                printf("send pack data failed, size: %d, err: %s\n", pac_len,
//                        strerror(errno));
//            }
//        }
//    }


    ret = en->encode_do(cvt_buf, cvt_len, &enc_buf, &enc_len,
            &ptype);
    if (ret < 0)
    {
        qDebug()<<"--- encode_do failed\n";
    //    break;
    }
    if (enc_len <= 0)
    {
//        qDebug()<<"!!! No encode data\n";
    //    continue;
    }
    // else
    //fwrite(enc_buf, 1, enc_len, dumpfile);
    // RTP pack and send

    rs->pack_put(enc_buf, enc_len);
    while(rs->pack_get(pac_buf, MAXDATASIZE, &pac_len) == 1)
    {

    }

    framecount++;
    }

    stopped = true;

// label->show();
// label->drawFrame();

//    QPixmap *pixImage = new QPixmap();
//    pixImage->loadFromData((uchar *)pp,sizeof(pp),0,Qt::AutoColor);
//    QPainter painter(this);
//    painter.begin(this);
//    painter.drawPixmap(0,0,QWidget::width(),QWidget::height(),*pixImage);
//    painter.end();
}

void CameraThread::stop()
{
    stopped = true;
}

