#ifndef VIDEO_H
#define VIDEO_H

#include "src/rtpsession.h"
#include "src/rtpsessionparams.h"
#include "src/rtpudpv4transmitter.h"
#include "src/rtpipv4address.h"
#include "src/rtptimeutilities.h"
#include "src/rtppacket.h"

#include <comdef.h>
#include <QDialog>
#include <QtGui>

#include <QImage>
#include <QTimer>

#include "rtpreceive.h"
#include "decode.h"
#include "au_decode.h"

using namespace jrtplib;
using namespace std;

namespace Ui {
class Video;
}

class Video : public QDialog
{
    Q_OBJECT

public:
    explicit Video(QWidget *parent = 0,RTPSession *session = 0,au_decode **audecode = 0);
    ~Video();

    uint32_t ssrc[2];


private:
    Ui::Video *ui;

    int ret;

    bool update1,update2;

    QTimer    *timer;

    RTPreceive *rr1;
    RTPreceive *rr2;
    decode *de1;
    decode *de2;
    au_decode *ad;

    int receive_bytes,de_len,totalsize1,totalsize2;
    char *recv_buf1,*recv_buf2,*recv_bufa;
    void *de_buf1,*de_buf2,*dis_buf1,*dis_buf2;
    bool marker;

    RTPSession *rtpsess;
    RTPPacket *rtppack;

    int status, timeout;

    void checkerror( int errorcode );

private slots:
    void paintEvent(QPaintEvent *);
    void readingframe();

signals:
    getframe();
};

#endif // VIDEO_H
