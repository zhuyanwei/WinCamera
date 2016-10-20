#ifndef cameraGET_H
#define cameraGET_H

#include <QObject>

#include <highgui.h>  //包含opencv库头文件
#include <cv.h>



namespace Ui {
    class cameraGet;
}

class cameraGet : public QObject
{
    Q_OBJECT

public:
    cameraGet();
    ~cameraGet();

    int opencamera(int width,int height);      // 打开摄像头
    int readFarme(IplImage **frame);       // 读取当前帧信息
    void closecamera();     // 关闭摄像头。
//    void takingPictures();  // 拍照

private:
    CvCapture *cam;// 视频获取结构， 用来作为视频获取函数的一个参数
    IplImage  *frame;//申请IplImage类型指针，就是申请内存空间来存放每一帧图像
};

#endif // cameraGET_H
