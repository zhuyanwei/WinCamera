#include "cameraget.h"
#include <QDebug>

cameraGet::cameraGet()
{
    cam = NULL;

}

/******************************
********* 打开摄像头 ***********
*******************************/
int cameraGet::opencamera(int width,int height)
{
    cam = cvCreateCameraCapture(0);//打开摄像头，从摄像头中获取视频
    cvSetCaptureProperty(cam,CV_CAP_PROP_FRAME_WIDTH,width);
    cvSetCaptureProperty(cam,CV_CAP_PROP_FRAME_HEIGHT,height);
//        qDebug()<<"Width"<<cvGetCaptureProperty(cam,CV_CAP_PROP_FRAME_WIDTH);
//        qDebug()<<"Height"<<cvGetCaptureProperty(cam,CV_CAP_PROP_FRAME_HEIGHT);
    return 0;
}

/*********************************
********* 读取摄像头信息 ***********
**********************************/
int cameraGet::readFarme(IplImage **frame)
{
    *frame = cvQueryFrame(cam);// 从摄像头中抓取并返回每一帧
    return 0;
    // 将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。
}

///*************************
//********* 拍照 ***********
//**************************/
//void cameraGet::takingPictures()
//{
//    frame = cvQueryFrame(cam);// 从摄像头中抓取并返回每一帧

//    // 将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。
//    QImage image = QImage((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();

//    ui->label_2->setPixmap(QPixmap::fromImage(image));  // 将图片显示到label上
//}

/*******************************
***关闭摄像头，释放资源，必须释放***
********************************/
void cameraGet::closecamera()
{
    cvReleaseCapture(&cam);//释放内存；
}

cameraGet::~cameraGet()
{

}
