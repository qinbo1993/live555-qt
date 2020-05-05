/**
* @file  main.cpp
* @brief main
* @author       Qinbo
* @date     2020-05-05
* @version  1.1.0
* @copyright Vincent
*/



#include <QCoreApplication>

#include "MediaRtspClient.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MediaRtspClient *pRtsp = new MediaRtspClient("rtsp://admin:123456QinBo@192.168.0.8/h264/ch1/main/av_stream", \
                                                 "admin", "123456QinBo");
    pRtsp->open();

    return 0;
    //return a.exec();
}

