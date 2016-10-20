#ifndef WIDGET_H
#define WIDGET_H

#include "camerathread.h"
#include "video.h"

#include "audioget.h"
#include "au_encode.h"
#include "audiothread.h"
#include "au_decode.h"

#include <QNetworkInterface>
#include <QWidget>
#include <QtGui>
#include <QImage>
#include <QTimer>     // 设置采集数据的间隔时间
#include <QMessageBox>
#include <QHostInfo>
#include <QUdpSocket>

namespace Ui {
    class Widget;
}

enum MessageType{Request,Callback,Callback2,Invite,CutIn,CutInCB};

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:

public slots:

private:
    Ui::Widget *ui;

    CameraThread *CT;
    Video *Vid;

    audioget *au;
    au_encode *ae;
    au_decode *ad;

    AudioThread *AT;
    void *ae_buf;
    int ae_size;

    QTimer *timer;

    RTPSession session;
    RTPSessionParams sessionparams;
    RTPUDPv4TransmissionParams transparams;
    int status;

    QUdpSocket *udpSocket;
    int port;

    int isStart;
    int isStart2;
    int isServer;

    uint32_t iplist[3];
    uint16_t portlist[3];
    uint32_t ssrclist[3];

    uint32_t QCip;
    uint16_t QCport;

    uint32_t m_ip;
    uint16_t m_port;

    bool CheckError(int rtperr);
    void add_dest(uint32_t dest_ip,uint16_t dest_port);

    void sendMessage(MessageType type,char* destip);


private slots:
    void paintEvent(QPaintEvent *);

    int processaudio();

    void on_open_clicked();
    void on_closeCam_clicked();
    void on_Connect_clicked();

    void processRequest();

    void on_adddest_clicked();
    void on_Initial_clicked();
    void on_invite_clicked();

signals:

};

#endif // WIDGET_H
