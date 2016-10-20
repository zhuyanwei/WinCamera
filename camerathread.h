#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include "cameraget.h"
#include "convert.h"
#include "encode.h"
#include "rtpsend.h"

#include <QMessageBox>

#include <QThread>

namespace Ui {
class CameraThread;
}

class CameraThread : public QThread
{
    Q_OBJECT

public:
    explicit CameraThread(RTPSession *session);
    ~CameraThread();

    IplImage *cap_frame;

    void stop();

private: 
    cameraGet *cp;
    convert *cv;
    encode *en;
    rtpsend *rs;

    int ret;
    bool stopped;

    void *cap_buf,*cvt_buf,*hd_buf,*enc_buf;
    char *pac_buf ;
    int cap_len, cvt_len, hd_len, enc_len, pac_len;
    enum encode::pic_t ptype;

    unsigned long framecount;
signals:
    captured();

protected:
  void run();
};



#endif // CAMERATHREAD_H
