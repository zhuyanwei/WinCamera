#include "rtpsend.h"
#include <QDebug>


rtpsend::rtpsend()
{
//    int Error;
//    WORD Version=MAKEWORD(2,2);
//    WSADATA WsaData;
//    Error=WSAStartup(Version,&WsaData);	      //Start up WSA
//    if(Error!=0)
//    {

//    }
//    else
//    {
//        if(LOBYTE(WsaData.wVersion)!=2||HIBYTE(WsaData.wHighVersion)!=2)
//        {
//            qDebug()<<"WSA error";
//            WSACleanup();
//        }
//    }
    timestamp_increse=0;
    ts_current=0;
}

rtpsend::~rtpsend()
{

}


//void rtpsend::SetRTPParams(RTPSession sess, uint32_t destip, uint16_t destport, uint16_t baseport)
//{
//    //RTP+RTCP库初始化SOCKET环境
//    sessionparams.SetOwnTimestampUnit(1.0/9000.0); //时间戳单位
//    sessionparams.SetAcceptOwnPackets(true);   //接收自己发送的数据包
//    sessionparams.SetUsePredefinedSSRC(true);  //设置使用预先定义的SSRC
//    sessionparams.SetPredefinedSSRC(SSRC);     //定义SSRC

//    transparams.SetPortbase(baseport);

//    status = sess.Create(sessionparams,&transparams);
//    CheckError(status);

//    destip = ntohl(destip);

//    RTPIPv4Address addr(destip,destport);
//    status = sess.AddDestination(addr);
//    CheckError(status);

//    //为发送H264包设置参数
//    //sess.SetParamsForSendingH264();

//}


int rtpsend::net_open(RTPSession *sess)
{
     session = sess;

//     SetRTPParams(session,dest_ip,dest_port,BASE_PORT);

//     uint8_t localip[]={127,0,0,1};
//     RTPIPv4Address addr(localip,dest_port);

//     status = session->AddDestination(addr);
//     if (status < 0)
//     {
//       std::cerr << RTPGetErrorString(status) << std::endl;
//       exit(-1);
//     }

//     RTPTime delay(0.040);
//     RTPTime starttime = RTPTime::CurrentTime();

    printf("+++ Network Opened\n");
    return 0;
}

void rtpsend::net_close()
{
//    ::close(this->sktfd);
//    WSACleanup();
    free(this);
    printf("+++ Network Closed\n");
}

//int rtpsend::net_send(char *data, int size)
//{
//    return ::send(this->sktfd, data, size, 0);
//}

//int rtpsend::net_recv(char *data, int size)
//{
//    return ::recv(this->sktfd, data, size, 0);
//}


U32 rtpsend::get_current_millisec(void)
{
    struct timeb tb;
    ftime(&tb);
    return 1000 * tb.time + tb.millitm;
}

int rtpsend::pack_open(int max_pkt_len,int ssrc)
{
    this->FU_index = 0;
    this->nalu.data = NULL;
    this->seq_num = 0;
    this->ts_current_sample = 0;
    this->max_pkt_len = max_pkt_len;
    this->ssrc = ssrc;
    this->ts_start_millisec = get_current_millisec();	// save the startup time

    printf("+++ Pack Opened\n");
    return 0;
}

void rtpsend::pack_put(void *inbuf, int isize)
{
    this->inbuf = inbuf;
    this->next_nalu_ptr = (char*)this->inbuf;
    this->inbuf_size = isize;
    this->FU_counter = 0;
    this->last_FU_size = 0;
    this->FU_index = 0;
    this->inbuf_complete = 0;
    this->nalu_complete = 1;    // start a new nalu
}

int rtpsend::is_start_code4(char *buf)
{
    if (buf[0] != 0 || buf[1] != 0 || buf[2] != 0 || buf[3] != 1)    // 0x00000001
        return 0;
    else
        return 1;
}

int rtpsend::is_start_code3(char *buf)
{
    if (buf[0] != 0 || buf[1] != 0 || buf[2] != 1)    // 0x000001
        return 0;
    else
        return 1;
}

int rtpsend::get_next_nalu()
{
    if (!this->next_nalu_ptr)    // reach data end, no next nalu
    {
        return 0;
    }

    if (is_start_code3(this->next_nalu_ptr))    // check 0x000001 first
    {
        this->nalu.startcodeprefix_len = 3;
    }
    else
    {
        if (is_start_code4(this->next_nalu_ptr))    // check 0x00000001
        {
            this->nalu.startcodeprefix_len = 4;
        }
        else
        {
            printf("!!! No any startcode found\n");
            return -1;
        }
    }

    // found the next start code
    int startcode_found = 0;
    char *cur_nalu_ptr = this->next_nalu_ptr;    // rotate, save the next ptr
    char *next_ptr = cur_nalu_ptr + this->nalu.startcodeprefix_len;    // skip current start code
    while (!startcode_found)
    {
        next_ptr++;

        if ((next_ptr - (char *) this->inbuf) >= this->inbuf_size)    // reach data end
        {
            this->next_nalu_ptr = NULL;    // no more nalus
            break;
        }

        if (is_start_code3(next_ptr) || is_start_code4(next_ptr))    // found the next start code
        {
            this->next_nalu_ptr = next_ptr;    // next ptr
            break;
        }
    }

    this->nalu.data = cur_nalu_ptr + this->nalu.startcodeprefix_len;    // exclude the start code
    this->nalu.len = next_ptr - cur_nalu_ptr
            - this->nalu.startcodeprefix_len;
    this->nalu.forbidden_bit = (this->nalu.data[0] & 0x80) >> 7;    // 1 bit, 0b1000 0000
    this->nalu.nal_reference_idc = (this->nalu.data[0] & 0x60) >> 5;    // 2 bit, 0b0110 0000
    this->nalu.nal_unit_type = (this->nalu.data[0] & 0x1f);    // 5 bit, 0b0001 1111

    return 1;
}

void rtpsend::dump_nalu(const nalu_t *nalu)
{
    if (!nalu) return;

    printf("*** nalu len: %d, ", nalu->len);
    printf("nal_unit_type: %x\n", nalu->nal_unit_type);
}

int rtpsend::pack_get(void *outbuf, int bufsize, int *outsize)
{
    int ret;
    if (this->inbuf_complete) return 0;
    memset(outbuf, 0, bufsize);    // !!! this is important, missing this may cause werid problems, like VLC displays nothing when the buf is small!
//    char *tmp_outbuf = (char *) outbuf;
//    // set common rtp header
//    rtp_header *rtp_hdr;

//    rtp_hdr = (rtp_header *) tmp_outbuf;
//    rtp_hdr->payload = H264;
//    rtp_hdr->version = 2;
//    rtp_hdr->marker = 0;
//    rtp_hdr->ssrc = htonl(this->ssrc);    // constant for a RTP session

    if (this->nalu_complete)    // current nalu complete, find the next nalu in inbuf
    {
        ret = get_next_nalu();
        if (ret <= 0)    // no more nalus
        {
            this->inbuf_complete = 1;
            return 0;
        }

//		dump_nalu(&this->nalu);

//        rtp_hdr->seq_no = htons(this->seq_num++);    // increase for every RTP packet
//        this->ts_current_sample = (U32) ((get_current_millisec() - this->ts_start_millisec) * 90.0);    // calculate the timestamp for a new NALU
//        rtp_hdr->timestamp = htonl(this->ts_current_sample);
        // this the new NALU
        if (this->nalu.len <= this->max_pkt_len)    // no need to fragment
        {
//            rtp_hdr->marker = 1;
            nalu_header *nalu_hdr;

//            nalu_hdr = (nalu_header *) (tmp_outbuf + 12);
            nalu_hdr =(nalu_header*)&sendbuf[0];

            nalu_hdr->F = this->nalu.forbidden_bit;
            nalu_hdr->NRI = this->nalu.nal_reference_idc;
            nalu_hdr->TYPE = this->nalu.nal_unit_type;
//            char *nalu_payload = tmp_outbuf + 13;    // 12 Bytes RTP header + 1 Byte NALU header
            *outsize = this->nalu.len + 12;
            if (bufsize < *outsize)    // check size
            {
                printf("--- buffer size %d < pack size %d\n", bufsize,
                        *outsize);
                abort();
            }

            nalu_payload=&sendbuf[1];

            memcpy(nalu_payload, this->nalu.data + 1, this->nalu.len - 1);    // exclude the nalu header

            if(nalu_hdr->TYPE==1 || nalu_hdr->TYPE==5)
                    {

                      status = session->SendPacket((void *)sendbuf,this->nalu.len,H264,true,90000/FRAMERATE);
                    }
            else
                    {
                        status = session->SendPacket((void *)sendbuf,this->nalu.len,H264,true,0);
                        //如果是6,7类型的包，不应该延时；之前有停顿，原因这在这
//                        continue;
                    }
                    //发送RTP格式数据包并指定负载类型为96
            if (status < 0)
                    {
                      qDebug()<<"error single NALU";
                      std::cerr << RTPGetErrorString(status) << std::endl;
                      exit(-1);
                    }

            this->nalu_complete = 1;
            return 1;
        }
        else    // fragment needed
        {

            // calculate the fragments
            if (this->nalu.len % this->max_pkt_len == 0)    // in case divide exactly
            {
                this->FU_counter = this->nalu.len
                        / this->max_pkt_len - 1;
                this->last_FU_size = this->max_pkt_len;
            }
            else
            {
                this->FU_counter = this->nalu.len
                        / this->max_pkt_len;
                this->last_FU_size = this->nalu.len
                        % this->max_pkt_len;
            }
            this->FU_index = 0;

            // it's the first FU
//            rtp_hdr->marker = 0;


//            fu_indicator *fu_ind = (fu_indicator *) (tmp_outbuf + 12);
            fu_ind =(fu_indicator*)&sendbuf[0];

            fu_ind->F = this->nalu.forbidden_bit;
            fu_ind->NRI = this->nalu.nal_reference_idc;
            fu_ind->TYPE = 28;    // FU_A

//            fu_header *fu_hdr = (fu_header *) (tmp_outbuf + 13);
            fu_hdr =(fu_header*)&sendbuf[1];

            fu_hdr->E = 0;
            fu_hdr->R = 0;
            fu_hdr->S = 1;    // start bit
            fu_hdr->TYPE = this->nalu.nal_unit_type;
//            char *nalu_payload = tmp_outbuf + 14;
            *outsize = this->max_pkt_len + 14;    // RTP header + FU indicator + FU header
            if (bufsize < *outsize)
            {
                printf("--- buffer size %d < pack size %d\n", bufsize,
                        *outsize);
                abort();
            }

            nalu_payload=&sendbuf[2];

            memcpy(nalu_payload, this->nalu.data + 1,
                    this->max_pkt_len);

            status = session->SendPacket((void *)sendbuf,this->max_pkt_len+2,H264,false,0);
                        if (status < 0)
                        {
                            qDebug()<<"error first FUs";
                          std::cerr << RTPGetErrorString(status) << std::endl;
                          exit(-1);
                        }


            this->nalu_complete = 0;    // not complete
            this->FU_index++;

            return 1;
        }
    }
    else    // send remaining FUs
    {
//        rtp_hdr->seq_no = htons(this->seq_num++);
//        rtp_hdr->timestamp = htonl(this->ts_current_sample);    // it's a continuation to the last NALU, no need to recalculate

        // check if it's the last FU
        if (this->FU_index == this->FU_counter)    // the last FU
        {

//            rtp_hdr->marker = 1;    // the last FU

//            fu_indicator *fu_ind = (fu_indicator *) (tmp_outbuf + 12);
            fu_ind =(fu_indicator*)&sendbuf[0];

            fu_ind->F = this->nalu.forbidden_bit;
            fu_ind->NRI = this->nalu.nal_reference_idc;
            fu_ind->TYPE = 28;

//            fu_header *fu_hdr = (fu_header *) (tmp_outbuf + 13);
            fu_hdr =(fu_header*)&sendbuf[1];

            fu_hdr->R = 0;
            fu_hdr->S = 0;
            fu_hdr->TYPE = this->nalu.nal_unit_type;
            fu_hdr->E = 1;    // the last EU
//            char *nalu_payload = tmp_outbuf + 14;
            *outsize = this->last_FU_size - 1 + 14;
            if (bufsize < *outsize)
            {
                printf("--- buffer size %d < pack size %d\n", bufsize,
                        *outsize);
                abort();
            }

            nalu_payload=&sendbuf[2];

            memcpy(nalu_payload,
                    this->nalu.data + 1
                            + this->FU_index * this->max_pkt_len,
                    this->last_FU_size - 1);    // minus the nalu header

            status = session->SendPacket((void *)sendbuf,this->last_FU_size+1,H264,true,90000/FRAMERATE);
                        if (status < 0)
                        {
                          qDebug()<<"error last FUs";
                          std::cerr << RTPGetErrorString(status) << std::endl;
                          exit(-1);
                        }

            this->nalu_complete = 1;    // this nalu is complete
            this->FU_index = 0;
            return 1;
        }
        else    // middle FUs
        {
//            rtp_hdr->marker = 0;

//            fu_indicator *fu_ind = (fu_indicator *) (tmp_outbuf + 12);
            fu_ind =(fu_indicator*)&sendbuf[0];

            fu_ind->F = this->nalu.forbidden_bit;
            fu_ind->NRI = this->nalu.nal_reference_idc;
            fu_ind->TYPE = 28;

//            fu_header *fu_hdr = (fu_header *) (tmp_outbuf + 13);
            fu_hdr =(fu_header*)&sendbuf[1];

            fu_hdr->R = 0;
            fu_hdr->S = 0;
            fu_hdr->TYPE = this->nalu.nal_unit_type;
            fu_hdr->E = 0;

//            char *nalu_payload = tmp_outbuf + 14;
            *outsize = this->max_pkt_len + 14;
            if (bufsize < *outsize)
            {
                printf("--- buffer size %d < pack size %d\n", bufsize,
                        *outsize);
                abort();
            }

            nalu_payload=&sendbuf[2];

            memcpy(nalu_payload,
                    this->nalu.data + 1
                            + this->FU_index * this->max_pkt_len,
                    this->max_pkt_len);

            status = session->SendPacket((void *)sendbuf,this->max_pkt_len+2,H264,false,0);
                        if (status < 0)
                        {
                          qDebug()<<"error middle FUs";
                          std::cerr << RTPGetErrorString(status) << std::endl;
                          exit(-1);
                        }

            this->FU_index++;
            return 1;
        }
    }
}


