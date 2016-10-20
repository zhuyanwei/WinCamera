#include"videothread.h"


#include<QDebug>

VideoThread::VideoThread()
{
    rr =new RTPreceive();
    de =new decode();

    receive_bytes = 0;
    totalsize = 0;
    marker = false;

    ret=de->decode_open();

    ret=de->convert_open();

//    ret=rr->InitiateWinsock(sess);

    recv_buf = (char *) malloc(MAXDATASIZE);

    stopped = false;
}

VideoThread::~VideoThread()
{
    free(recv_buf);
    rr->net_close();
    de->decode_close();
}


void VideoThread::run()
{
    while (!stopped)
    {
        #ifndef RTP_SUPPORT_THREAD
        rr->status = rr->rtpsess->Poll();
        rr->checkerror( rr->status );
        #endif // RTP_SUPPORT_THREAD

          rr->rtpsess->BeginDataAccess();

          if( rr->rtpsess->GotoFirstSourceWithData() )
          {
           do
           {

                  while( ( rr->rtppack = rr->rtpsess->GetNextPacket() ) != NULL )
                  {


                   receive_bytes = rr->rtppack->GetPayloadLength();
                   totalsize += receive_bytes;
              //     printf( "recv ... data size: %ldKB(%ldB)   total size: %ldKB(%ldB)\n",
              //      (receive_bytes / 1024), receive_bytes,
              //      (totalsize / 1024), totalsize );
                   recv_buf = (char *)rr->rtppack->GetPayloadData();
                   marker = rr->rtppack->HasMarker();

                   if(rr->rtp_unpackage(recv_buf,receive_bytes,marker,&de_buf,&de_len) == 1)
                   {
                       if (de->decode_do(de_buf,de_len,&dis_buf) ==1)
                       {
                           emit getframe();

              //         writesize = fwrite( recvdata, 1, recvsize, fp );
              //         if( writesize != recvsize )
              //         {
              //          printf( "try to write [%ldKB(%ldB)/%ldKB(%ldB)] data into file....ERROR\n",
              //            (writesize / 1024), writesize,
              //            (recvsize / 1024), recvdata );
              //         }
                       }rr->rtpsess->DeletePacket( rr->rtppack );
                   }
                  }
           }while( rr->rtpsess->GotoNextSourceWithData() );
          }
          rr->rtpsess->EndDataAccess();
    }

    stopped = true;
}


void VideoThread::stop()
{
    stopped = true;
}
