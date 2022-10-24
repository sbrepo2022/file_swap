#-------------------------------------------------
#
# Project created by QtCreator 2018-04-26T21:07:35
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileSwap
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp\
        MoveTracker.cpp\
        UdpConnection.cpp\
        TcpConnection.cpp\
        FSData.cpp\
        FSCore.cpp \
        overlay.cpp \
    messageoverlay.cpp \
    passwordoverlay.cpp \
    FileDownloading.cpp

HEADERS += \
        mainwindow.h\
        MoveTracker.h\
        UdpConnection.h\
        TcpConnection.h\
        FSData.h\
        FSCore.h \
        overlay.h \
    messageoverlay.h \
    passwordoverlay.h \
    FileDownloading.h

FORMS += \
        mainwindow.ui \
    overlay.ui \
    messageoverlay.ui \
    passwordoverlay.ui

RESOURCES += \
        resources.qrc \
    file_icons_resource.qrc

RC_FILE = res.rc
