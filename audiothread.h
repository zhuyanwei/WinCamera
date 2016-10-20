#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include "audioget.h"

#include <QMessageBox>
#include <au_encode.h>
#include <QThread>

namespace Ui {
class AudioThread;
}

class AudioThread : public QThread
{
    Q_OBJECT

public:
    explicit AudioThread(audioget *au);
    ~AudioThread();

    void stop();

private: 
    audioget *audio;

    void *au_buf;
    int au_size;

    int ret;
    bool stopped;

signals:
    captured();

protected:
  void run();
};



#endif // CAMERATHREAD_H
