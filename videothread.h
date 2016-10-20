#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include "rtpreceive.h"
#include "decode.h"

#include <QMessageBox>

#include <QThread>


namespace Ui {
class VideoThread;
}

class VideoThread : public QThread
{
    Q_OBJECT

public:
    explicit VideoThread();
    ~VideoThread();

    void *dis_buf;

    void stop();

private:
    RTPreceive *rr;
    decode *de;

    int ret;
    bool stopped;

    int receive_bytes,de_len,totalsize;
    char *recv_buf;
    void *de_buf;
    bool marker;

signals:
    getframe();

protected:
    void run();

};

#endif // VIDEOTHREAD_H
