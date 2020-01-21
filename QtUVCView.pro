QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cameraview.cpp \
    main.cpp \
    mainwindow.cpp \
    uvccapture.cpp \
    uvccapturesettings.cpp \
    uvccontrolwidget.cpp

HEADERS += \
    cameraview.h \
    mainwindow.h \
    utlist.h \
    uvccapture.h \
    uvccapturesettings.h \
    uvccontrolwidget.h

OPENCV_PATH = /usr/local/Cellar/opencv/4.2.0_1
LIBS += -L/$$OPENCV_PATH/lib/ \
            -lopencv_core \
            -lopencv_bgsegm \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lopencv_ml \
            -lopencv_video \
            -lopencv_features2d \
            -lopencv_calib3d \
            -lopencv_objdetect \
            #-lopencv_contrib \
            #-lopencv_legacy \
            -lopencv_flann \
            -lopencv_videoio

INCLUDEPATH += $$OPENCV_PATH/include/opencv4
DEPENDPATH += $$OPENCV_PATH/include

LIBUVC_PATH = /usr/local/Cellar/libuvc/0.0.6
LIBS += -L/$$LIBUVC_PATH/lib/ \
            -luvc

INCLUDEPATH += $$LIBUVC_PATH/include
DEPENDPATH += $$LIBUVC_PATH/include

FORMS += \
    mainwindow.ui \
    uvccapturesettings.ui \
    uvccontrolwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
