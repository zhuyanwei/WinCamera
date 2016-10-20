#-------------------------------------------------
#
# Project created by QtCreator 2015-08-23T13:45:54
#
#-------------------------------------------------

QT       += core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WinCamera
TEMPLATE = app

SOURCES += main.cpp\
        cameraget.cpp \
    convert.cpp \
    encode.cpp \
    widget.cpp\
D:\Qt\MINGW\share\gettext\intl\localcharset.c \
    src/jmutex.cpp \
    src/jthread.cpp \
    src/rtcpapppacket.cpp \
    src/rtcpbyepacket.cpp \
    src/rtcpcompoundpacket.cpp \
    src/rtcpcompoundpacketbuilder.cpp \
    src/rtcppacket.cpp \
    src/rtcppacketbuilder.cpp \
    src/rtcprrpacket.cpp \
    src/rtcpscheduler.cpp \
    src/rtcpsdesinfo.cpp \
    src/rtcpsdespacket.cpp \
    src/rtcpsrpacket.cpp \
    src/rtpbyteaddress.cpp \
    src/rtpcollisionlist.cpp \
    src/rtpdebug.cpp \
    src/rtperrors.cpp \
    src/rtpexternaltransmitter.cpp \
    src/rtpinternalsourcedata.cpp \
    src/rtpipv4address.cpp \
    src/rtpipv6address.cpp \
    src/rtplibraryversion.cpp \
    src/rtppacket.cpp \
    src/rtppacketbuilder.cpp \
    src/rtppollthread.cpp \
    src/rtprandom.cpp \
    src/rtprandomrand48.cpp \
    src/rtprandomrands.cpp \
    src/rtprandomurandom.cpp \
    src/rtpsession.cpp \
    src/rtpsessionparams.cpp \
    src/rtpsessionsources.cpp \
    src/rtpsourcedata.cpp \
    src/rtpsources.cpp \
    src/rtptimeutilities.cpp \
    src/rtpudpv4transmitter.cpp \
    src/rtpudpv6transmitter.cpp \
    rtpsend.cpp \
    camerathread.cpp \
    decode.cpp \
    rtpreceive.cpp \
    video.cpp \
    au_decode.cpp \
    au_encode.cpp \
    audioget.cpp \
    audiothread.cpp

HEADERS  += cameraget.h \
    comdef.h \
    config.h \
    convert.h \
    encode.h \
    ffmpeg_common.h \
    rtpsend.h \
    widget.h \
    src/jmutex.h \
    src/jmutexautolock.h \
    src/jthread.h \
    src/jthreadconfig.h \
    src/rtcpapppacket.h \
    src/rtcpbyepacket.h \
    src/rtcpcompoundpacket.h \
    src/rtcpcompoundpacketbuilder.h \
    src/rtcppacket.h \
    src/rtcppacketbuilder.h \
    src/rtcprrpacket.h \
    src/rtcpscheduler.h \
    src/rtcpsdesinfo.h \
    src/rtcpsdespacket.h \
    src/rtcpsrpacket.h \
    src/rtcpunknownpacket.h \
    src/rtpaddress.h \
    src/rtpbyteaddress.h \
    src/rtpcollisionlist.h \
    src/rtpconfig.h \
    src/rtpdebug.h \
    src/rtpdefines.h \
    src/rtperrors.h \
    src/rtpexternaltransmitter.h \
    src/rtphashtable.h \
    src/rtpinternalsourcedata.h \
    src/rtpipv4address.h \
    src/rtpipv4destination.h \
    src/rtpipv6address.h \
    src/rtpipv6destination.h \
    src/rtpkeyhashtable.h \
    src/rtplibraryversion.h \
    src/rtpmemorymanager.h \
    src/rtpmemoryobject.h \
    src/rtppacket.h \
    src/rtppacketbuilder.h \
    src/rtppollthread.h \
    src/rtprandom.h \
    src/rtprandomrand48.h \
    src/rtprandomrands.h \
    src/rtprandomurandom.h \
    src/rtprawpacket.h \
    src/rtpsession.h \
    src/rtpsessionparams.h \
    src/rtpsessionsources.h \
    src/rtpsourcedata.h \
    src/rtpsources.h \
    src/rtpstructs.h \
    src/rtptimeutilities.h \
    src/rtptransmitter.h \
    src/rtptypes.h \
    src/rtptypes_win.h \
    src/rtpudpv4transmitter.h \
    src/rtpudpv6transmitter.h \
    camerathread.h \
    decode.h \
    rtpreceive.h \
    video.h \
    au_decode.h \
    au_encode.h \
    audioget.h \
    audiothread.h

FORMS    += widget.ui \
    video.ui

INCLUDEPATH+=D:\OpenCV\install\include\opencv \
D:\OpenCV\install\include \
D:\FFMPEG+\include \
D:\portaudio\include \

LIBS+= D:\OpenCV\install\bin\libopencv_highgui231.dll\
D:\OpenCV\install\bin\libopencv_core231.dll\
D:\FFMPEG+\lib\libavformat.a \
D:\FFMPEG+\lib\libavdevice.a \
D:\FFMPEG+\lib\libavcodec.a \
D:\FFMPEG+\lib\libavutil.a \
D:\FFMPEG+\lib\libswscale.a \
D:\FFMPEG+\lib\libswresample.a \
D:\FFMPEG+\x264\lib\libx264.a \
D:\FFMPEG+\faac\lib\libfaac.a \
D:\Qt\Qt5.4.1\Tools\mingw491_32\i686-w64-mingw32\lib\libz.a \

LIBS+= -LD:\portaudio\lib\Win32\ReleaseMinDependency -lportaudio_x86

LIBS += -lWs2_32
