QT += core gui
TARGET = QUVCView
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
    fpstimer.cpp \
    main.cpp \
    mainwindow.cpp \
    uvccapture.cpp \
    uvccapturecontrols.cpp \
    uvccapturesettings.cpp \
    uvccontrolwidget.cpp \
    videowriterwidget.cpp

HEADERS += \
    cameraview.h \
    fpstimer.h \
    mainwindow.h \
    utlist.h \
    uvccapture.h \
    uvccapturecontrols.h \
    uvccapturesettings.h \
    uvccontrolwidget.h \
    videowriterwidget.h

FORMS += \
    mainwindow.ui \
    uvccapturesettings.ui \
    uvccontrolwidget.ui \
    videowriterwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

macx {
    ICON = icons/QUVCView.icns
    QMAKE_INFO_PLIST = Info.plist
    CONFIG += app_bundle

    #include OpenCV and automatically select version
    OPENCV_PATH = $$files(/usr/local/Cellar/opencv/*)
    message(Using OpenCV at: $$OPENCV_PATH)

    LIBS += -L/$$OPENCV_PATH/lib/ \
            -lopencv_core \
            -lopencv_imgproc \
            -lopencv_videoio

    INCLUDEPATH += $$OPENCV_PATH/include/opencv4
    DEPENDPATH += $$OPENCV_PATH/include

    LIBUVC_PATH = $$files(/usr/local/Cellar/libuvc/*)
    message(Using libUVC at: $$LIBUVC_PATH)
    LIBS += -L/$$LIBUVC_PATH/lib/ \
            -luvc

    INCLUDEPATH += $$LIBUVC_PATH/include
    DEPENDPATH += $$LIBUVC_PATH/include
}
