#include "rtpreceive.h"
#include <QDebug>

RTPreceive::RTPreceive()
{

}

RTPreceive::~RTPreceive()
{
//    rtpsess->BYEDestroy(RTPTime(10,0),0,0);
}



int RTPreceive::InitiateWinsock(RTPSession *sess)
{

     rtpsess = sess;

     return 1;
}

//int  RTPreceive::OpenBitstreamFile (char *fn)
//{
//    if (NULL == (poutfile = fopen(fn, "wb")))
//    {
//        printf("Error: Open input file error\n");
//        getchar();
//    }
//    return 1;
//}

//NALU_t * RTPreceive::AllocNALU(int buffersize)
//{
//    NALU_t * n;

//    if ((n = (NALU_t*)calloc (1, sizeof(NALU_t))) == NULL)
//    {
//        printf("AllocNALU Error: Allocate Meory To NALU_t Failed ");
//        exit(0);
//    }
//    return n;
//}

void RTPreceive::net_close()
{
//    ::close(this->sktfd);
    free(this);
    printf("+++ Network Closed\n");
}


void RTPreceive::FreeNALU(NALU_t *n)
{
    if (n)
    {
        free (n);
        n = NULL;
    }
}

int RTPreceive::rtp_unpackage(char *bufIn, int len,bool marker, void **bufOut, int *olen)
{
    uchar recvbuf[MAXDATASIZE];
    NALU_HEADER  * nalu_hdr = NULL;
    FU_INDICATOR * fu_ind   = NULL;
    FU_HEADER	 * fu_hdr   = NULL;

    NALU_t * n  = NULL;

//    int total_bytes = 0;                 //当前包传出的数据
//    static int total_recved = 0;         //一共传输的数据
//    int fwrite_number = 0;               //存入文件的数据长度

    static char encodebuf[WIDTH*HEIGHT*3];      //要进行编码的buf
    static int pencode_number = 0;       //要进行编码的buf内部指针 应用于分片模式

    unsigned char F_N;
    unsigned char NRI_N;
    unsigned char TYPE_N;

    int NALFinished = 0;

    *olen=0;

    memcpy(recvbuf,bufIn, len);          //复制rtp包

    if ((nalu_hdr = (NALU_HEADER *)malloc(sizeof(NALU_HEADER))) == NULL)
    {
//        printf("NALU_HEADER MEMORY ERROR\n");
    }

    //begin nal_hdr

    if ((n = (NALU_t*)calloc (1, sizeof(NALU_t))) == NULL)
    {
//        printf("AllocNALU Error: Allocate Meory To NALU_t Failed ");
        exit(0);
    }

//    if (!(n = AllocNALU(800000)))          //为结构体nalu_t及其成员buf分配空间。返回值为指向nalu_t存储空间的指针
//    {
//        printf("NALU_t MMEMORY ERROR\n");
//    }

//	nalu_hdr =(NALU_HEADER*)&recvbuf[12];                        //网络传输过来的字节序 ，当存入内存还是和文档描述的相反，只要匹配网络字节序和文档描述即可传输正确。
    memcpy(nalu_hdr, &recvbuf[0], sizeof(NALU_HEADER));

/*    printf("forbidden_zero_bit: %d\n",nalu_hdr->F);      */        //网络传输中的方式为：F->NRI->TYPE.. 内存中存储方式为 TYPE->NRI->F (和nal头匹配)。
    n->forbidden_bit= nalu_hdr->F << 7;                          //内存中的字节序。
//    printf("nal_reference_idc:  %d\n",nalu_hdr->NRI);
    n->nal_reference_idc = nalu_hdr->NRI << 5;
//    printf("nal 负载类型:       %d\n",nalu_hdr->TYPE);
    n->nal_unit_type = nalu_hdr->TYPE;

    //end nal_hdr
    //////////////////////////////////////////////////////////////////////////
    //开始解包
    if ( nalu_hdr->TYPE  == 0)
    {
//        printf("这个包有错误，0无定义\n");
    }
    else if ( nalu_hdr->TYPE >0 &&  nalu_hdr->TYPE < 24)  //单包
    {
//        qDebug()<<"当前包为单包，类型："<<nalu_hdr->TYPE;

    //	putc(0x00, poutfile);
    //	putc(0x00, poutfile);
    //	putc(0x00, poutfile);
    //	putc(0x01, poutfile);

    //	fwrite(nalu_hdr,1,1,poutfile);

    //	fwrite_number = fwrite(p->payload,1,p->paylen,poutfile);

        encodebuf[0] = 0x00;
        encodebuf[1] = 0x00;
        encodebuf[2] = 0x00;
        encodebuf[3] = 0x01;
        //encodebuf[4] = (char)nalu_hdr;
        F_N = nalu_hdr->F << 7;
        NRI_N = nalu_hdr->NRI << 5;
        TYPE_N = nalu_hdr->TYPE;
        encodebuf[4] = F_N | NRI_N |TYPE_N;
        memcpy(encodebuf + 5 ,&recvbuf[1],len-1);
//        fwrite_number = fwrite(encodebuf ,1,p->paylen + 5,poutfile);
        *olen = len + 4;
        *bufOut = encodebuf;

        if (nalu_hdr->TYPE==7 || nalu_hdr->TYPE ==8)
        {
//            for (int i=0;i<p->paylen+5;i++)
//                qDebug()<<hex<<(uchar)encodebuf[i];
            NALFinished = 2;
        }
        else NALFinished = 1;
    }
//    else if ( nalu_hdr->TYPE == 24)                    //STAP-A   单一时间的组合包
//    {
//        printf("当前包为STAP-A\n");
//    }
//    else if ( nalu_hdr->TYPE == 25)                    //STAP-B   单一时间的组合包
//    {
//        printf("当前包为STAP-B\n");
//    }
//    else if (nalu_hdr->TYPE == 26)                     //MTAP16   多个时间的组合包
//    {
//        printf("当前包为MTAP16\n");
//    }
//    else if ( nalu_hdr->TYPE == 27)                    //MTAP24   多个时间的组合包
//    {
//        printf("当前包为MTAP24\n");
//    }
    else if ( nalu_hdr->TYPE == 28)                    //FU-A分片包，解码顺序和传输顺序相同
    {
        if ((fu_ind = (FU_INDICATOR *)malloc(sizeof(FU_INDICATOR))) == NULL)
        {
//            printf("FU_INDICATOR MEMORY ERROR\n");
        }

        if ((fu_hdr = (FU_HEADER *)malloc(sizeof(FU_HEADER))) == NULL)
        {
//            printf("FU_HEADER MEMORY ERROR\n");
        }

        //fu_ind=(FU_INDICATOR*)&recvbuf[12];
        memcpy(fu_ind, &recvbuf[0], sizeof(FU_INDICATOR));
//        printf("FU_INDICATOR->F     :%d\n",fu_ind->F);
        n->forbidden_bit = fu_ind->F << 7;
//        printf("FU_INDICATOR->NRI   :%d\n",fu_ind->NRI);
        n->nal_reference_idc = fu_ind->NRI << 5;
//        printf("FU_INDICATOR->TYPE  :%d\n",fu_ind->TYPE);
        n->nal_unit_type = fu_ind->TYPE;

    //	fu_hdr=(FU_HEADER*)&recvbuf[13];
        memcpy(fu_hdr, &recvbuf[1], sizeof(FU_HEADER));
//        printf("FU_HEADER->S        :%d\n",fu_hdr->S);
//        printf("FU_HEADER->E        :%d\n",fu_hdr->E);
//        printf("FU_HEADER->R        :%d\n",fu_hdr->R);
//        printf("FU_HEADER->TYPE     :%d\n",fu_hdr->TYPE);
        n->nal_unit_type = fu_hdr->TYPE;               //应用的是FU_HEADER的TYPE

        if (marker == 1)                      //分片包最后一个包
        {
//            qDebug()<<"当前包为FU-A分片包最后一个包\n";
        //	fwrite_number = fwrite(p->payload,1,p->paylen,poutfile);

            memcpy(encodebuf + pencode_number ,&recvbuf[2],len - 2);
//            fwrite_number = fwrite(encodebuf,1,pencode_number + p->paylen,poutfile);

            *olen = pencode_number + len - 2;
            *bufOut = encodebuf;

            pencode_number = 0;
            NALFinished = 1;
        }
        else if (marker == 0)                 //分片包 但不是最后一个包
        {
            if (fu_hdr->S == 1)                        //分片的第一个包
            {
                unsigned char F;
                unsigned char NRI;
                unsigned char TYPE;
                unsigned char nh;
//               qDebug()<<"当前包为FU-A分片包第一个包\n";
            //	putc(0x00, poutfile);
            //	putc(0x00, poutfile);
            //	putc(0x00, poutfile);
            //	putc(0x01, poutfile);

                F = fu_ind->F << 7;
                NRI = fu_ind->NRI << 5;
                TYPE = fu_hdr->TYPE;                                            //应用的是FU_HEADER的TYPE
                //nh = n->forbidden_bit|n->nal_reference_idc|n->nal_unit_type;  //二进制文件也是按 大字节序存储
                nh = F | NRI | TYPE;

            //	putc(nh,poutfile);

            //	fwrite_number = fwrite(p->payload,1,p->paylen,poutfile);
                encodebuf[0] = 0x00;
                encodebuf[1] = 0x00;
                encodebuf[2] = 0x00;
                encodebuf[3] = 0x01;
                encodebuf[4] = nh;
                memcpy(encodebuf + 5 ,&recvbuf[2],len - 2);
                pencode_number +=  (5 + len - 2);

                *olen = pencode_number;
                *bufOut = encodebuf;
            }
            else                                      //如果不是第一个包
            {
//                qDebug()<<"当前包为FU-A分片包\n";

            //	fwrite_number = fwrite(p->payload,1,p->paylen,poutfile);

                memcpy(encodebuf + pencode_number ,&recvbuf[2],len - 2);
                pencode_number += len - 2;

                *olen = pencode_number;
                *bufOut = encodebuf;

            }
            NALFinished = 0;
        }
    }
//    else if ( nalu_hdr->TYPE == 29)                //FU-B分片包，解码顺序和传输顺序相同
//    {
//        if (rtp_hdr->marker == 1)                  //分片包最后一个包
//        {
//            printf("当前包为FU-B分片包最后一个包\n");

//        }
//        else if (rtp_hdr->marker == 0)             //分片包 但不是最后一个包
//        {
//            printf("当前包为FU-B分片包\n");
//        }
//    }
    else
    {
        printf("这个包有错误，30-31 没有定义\n");
    }

    memset(recvbuf,0,MAXDATASIZE);

    if (nalu_hdr)
    {
        free(nalu_hdr);
        nalu_hdr = NULL;
    }
    if (fu_ind)
    {
        free(fu_ind);
        fu_ind = NULL;
    }
    if (fu_hdr)
    {
        free(fu_hdr);
        fu_hdr = NULL;
    }
    FreeNALU(n);
    //结束解包
    //////////////////////////////////////////////////////////////////////////

    if (NALFinished == 0)
    {
        return 0;
    }
    else if (NALFinished == 1)
        return 1;
    else if (NALFinished == 2)
        return 1;
    else return -1;
}



//void RTPreceive::pgm_save(unsigned char *buf,int wrap, int xsize,int ysize,char *filename)
//{
//	FILE *f;
//	int i;

//	f=fopen(filename,"ab+");
//	for(i=0;i<ysize;i++)
//	{
//		fwrite(buf + i * wrap, 1, xsize, f );
//	}
//	fclose(f);
//}
//}




