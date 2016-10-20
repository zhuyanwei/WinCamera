#ifndef RTPRECEIVE_H
#define RTPRECEIVE_H

#include "src/rtpsession.h"
#include "src/rtpsessionparams.h"
#include "src/rtpudpv4transmitter.h"
#include "src/rtpipv4address.h"
#include "src/rtptimeutilities.h"
#include "src/rtppacket.h"

#include <comdef.h>
#include <QObject>
#include <QUdpSocket>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <conio.h>
#include <WinSock2.h>
#include <iostream>

using namespace jrtplib;
using namespace std;

class QUdpSocket;


typedef struct
{
    unsigned char forbidden_bit;           //! Should always be FALSE
    unsigned char nal_reference_idc;       //! NALU_PRIORITY_xxxx
    unsigned char nal_unit_type;           //! NALU_TYPE_xxxx
    unsigned int startcodeprefix_len;      //! 预留
    unsigned int len;                      //! 预留
    unsigned int max_size;                 //! 预留
    unsigned char * buf;                   //! 预留
    unsigned int lost_packets;             //! 预留
} NALU_t;

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
*/
typedef struct
{
    //byte 0
    unsigned char TYPE:5;
    unsigned char NRI:2;
    unsigned char F:1;
} NALU_HEADER; // 1 BYTE

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
*/
typedef struct
{
    //byte 0
    unsigned char TYPE:5;
    unsigned char NRI:2;
    unsigned char F:1;
} FU_INDICATOR; // 1 BYTE

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|S|E|R|  Type   |
+---------------+
*/
typedef struct
{
    //byte 0
    unsigned char TYPE:5;
    unsigned char R:1;
    unsigned char E:1;
    unsigned char S:1;          //分片包第一个包 S = 1 ,其他= 0 。
} FU_HEADER;   // 1 BYTES


class RTPreceive : public QObject
{
    Q_OBJECT

public:
    RTPreceive(void);
    ~RTPreceive(void);

    RTPSession *rtpsess;
    RTPSessionParams sessionparams;
    RTPUDPv4TransmissionParams transparams;
    unsigned short localport;

    int status, timeout;

    RTPPacket *rtppack;

    void FreeNALU(NALU_t *n);                                                         //释放nal 资源
/*    NALU_t *AllocNALU(int buffersize);         */                                       //分配nal 资源
    int InitiateWinsock(RTPSession *sess);
    int rtp_unpackage(char *bufIn, int len, bool marker, void **bufOut, int *olen);
    void net_close();

    void checkerror( int errorcode );

    QUdpSocket *udpSocket;
    int port;

private:

signals:

public slots:
};

#endif // RTPRECEIVE_H
