#include "video.h"
#include "ui_video.h"

#include<QDebug>

void Video::checkerror( int errorcode )
{
 if( errorcode < 0 )
 {
  std::cout << "ERROR:" << RTPGetErrorString( errorcode ) << std::endl;
 }
}

Video::Video(QWidget *parent, RTPSession *session, au_decode **audecode) :
    QDialog(parent),
    ui(new Ui::Video)
{
    ui->setupUi(this);

    rtpsess = session;

    timer   = new QTimer(this);

    // 初始化
    rr1 =new RTPreceive();
    rr2 =new RTPreceive();
    de1 =new decode();
    de2 =new decode();
    ad = *audecode;

    receive_bytes = 0;
    totalsize1 = 0;
    totalsize2 = 0;
    marker = false;

    ret=de1->decode_open();
    ret=de1->convert_open();
    ret=de2->decode_open();
    ret=de2->convert_open();

    recv_buf1 = (char *) malloc(MAXDATASIZE);
    recv_buf2 = (char *) malloc(MAXDATASIZE);
    recv_bufa = (char *) malloc(MAXDATASIZE);

    connect(this, SIGNAL(getframe()), this, SLOT(update()));
    connect(timer, SIGNAL(timeout()), this, SLOT(readingframe()));
    timer->start(1);

    update1 = false;
    update2 = false;

}

Video::~Video()
{
    de1->decode_close();
    rr1->net_close();
    de2->decode_close();
    rr2->net_close();

    free(recv_buf1);
    free(recv_buf2);
    delete ui;
}

void Video::paintEvent(QPaintEvent *)
{
    if (update1)
    {
        QImage image = QImage((const uchar*)dis_buf1,WIDTH,HEIGHT, QImage::Format_RGB888).rgbSwapped();
        ui->label->setPixmap(QPixmap::fromImage(image));
        update1 = false;
    }

    if (update2)
    {
        QImage image2 = QImage((const uchar*)dis_buf2,WIDTH,HEIGHT, QImage::Format_RGB888).rgbSwapped();
        ui->label2->setPixmap(QPixmap::fromImage(image2));
        update2 = false;
    }

}

void Video::readingframe()
{

#ifndef RTP_SUPPORT_THREAD
    status = rtpsess->Poll();
    checkerror( status );
#endif // RTP_SUPPORT_THREAD

    rtpsess->BeginDataAccess();
  if( rtpsess->GotoFirstSourceWithData() )
  {
   do
   {
          while( ( rtppack = rtpsess->GetNextPacket() ) != NULL )
          {
           if (rtppack->GetPayloadType() == AAC)
           {
//               QDateTime t;
//               QTime time;
//               QDate date;
//               t.setTime(time.currentTime());
//               t.setDate(date.currentDate());
//               qDebug()<<t.toString("yyyy:MM:dd:hh:mm:ss");
//               qDebug()<<"audio"<<rtppack->GetTimestamp();
               receive_bytes = rtppack->GetPayloadLength();
               recv_bufa = (char *)rtppack->GetPayloadData();
               ad->decode_do(recv_bufa,receive_bytes);
           }
           else if (rtppack->GetPayloadType() == H264)
           {
           uint32_t packssrc = rtppack->GetSSRC();
//           QDateTime t;
//           QTime time;
//           QDate date;
//           t.setTime(time.currentTime());
//           t.setDate(date.currentDate());
//           qDebug()<<t.toString("yyyy:MM:dd:hh:mm:ss");
//           qDebug()<<"video"<<rtppack->GetTimestamp();
           int number=0;
           if (packssrc == ssrc[0]) number = 1;
           else if(packssrc == ssrc[1]) number = 2;

           switch (number)
           {
           case 1:
           {
           receive_bytes = rtppack->GetPayloadLength();
           totalsize1 += receive_bytes;
      //     printf( "recv ... data size: %ldKB(%ldB)   total size: %ldKB(%ldB)\n",
      //      (receive_bytes / 1024), receive_bytes,
      //      (totalsize / 1024), totalsize );
           recv_buf1 = (char *)rtppack->GetPayloadData();
           marker = rtppack->HasMarker();


           if(rr1->rtp_unpackage(recv_buf1,receive_bytes,marker,&de_buf1,&de_len) == 1)
           {
               if (de1->decode_do(de_buf1,de_len,&dis_buf1) ==1)
                  {
                   update1 = true;
                   emit getframe();
                   }

      //         writesize = fwrite( recvdata, 1, recvsize, fp );
      //         if( writesize != recvsize )
      //         {
      //          printf( "try to write [%ldKB(%ldB)/%ldKB(%ldB)] data into file....ERROR\n",
      //            (writesize / 1024), writesize,
      //            (recvsize / 1024), recvdata );
      //         }
              }
           break;
           }
           case 2:
           {
           receive_bytes = rtppack->GetPayloadLength();
           totalsize2 += receive_bytes;
      //     printf( "recv ... data size: %ldKB(%ldB)   total size: %ldKB(%ldB)\n",
      //      (receive_bytes / 1024), receive_bytes,
      //      (totalsize / 1024), totalsize );
           recv_buf2 = (char *)rtppack->GetPayloadData();
           marker = rtppack->HasMarker();


           if(rr2->rtp_unpackage(recv_buf2,receive_bytes,marker,&de_buf2,&de_len) == 1)
           {
               if (de2->decode_do(de_buf2,de_len,&dis_buf2) ==1)
                  {
                   update2 = true;
                   emit getframe();
                   }

      //         writesize = fwrite( recvdata, 1, recvsize, fp );
      //         if( writesize != recvsize )
      //         {
      //          printf( "try to write [%ldKB(%ldB)/%ldKB(%ldB)] data into file....ERROR\n",
      //            (writesize / 1024), writesize,
      //            (recvsize / 1024), recvdata );
      //         }
               }
           break;
           }
           }
           }
           rtpsess->DeletePacket( rtppack );
          }
   }while( rtpsess->GotoNextSourceWithData() );
  }
  rtpsess->EndDataAccess();




}
