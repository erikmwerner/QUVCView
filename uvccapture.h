#ifndef UVCCAPTURE_H
#define UVCCAPTURE_H

#include <QObject>
#include "libuvc/libuvc.h"
#include <opencv2/opencv.hpp>
#include <QRect>

class UVCCapture : public QObject
{
    Q_OBJECT

public:

    struct UVCCaptureDescriptor {
        uint16_t vendorID;
        uint16_t productID;
        uint16_t UVC_level;
        QString serial;
        QString manufacturer;
        QString productName;
        uvc_device_t* dev;
    };

    struct UVCCaptureProperties {
        uvc_frame_format format;
        QString formatString;
        QRect frameRect;
        int frameInterval;
        UVCCaptureProperties():
            format(UVC_FRAME_FORMAT_YUYV),
            formatString("YUYV"),
            frameRect(QRect(0,0,640,480)),
            frameInterval(333333) {}
    };


public:
    UVCCapture(QObject *parent = nullptr);
    ~UVCCapture();
    int initUVC();
    void closeUVC();
    void closeDevice();

    uvc_device_handle_t** handlePtr() {return &m_devh;}

    void startStream();
public slots:
    void findDevices();
    int openDevice(int vid = 0, int pid = 0, QString serial_number = QString());
    void checkDeviceCapabilities(uvc_device_handle_t *devh);
    void openDevicePtr(uvc_device_t *dev);
    void setCaptureProperties(UVCCaptureProperties properties);

signals:

    void frameAvailable(const cv::Mat &source, const int frame_number);

    //< only announce frame size changes when the frame has changed
    void frameRectChanged(const QRect &rect);

    void foundDevices(QVector<UVCCaptureDescriptor> devices);

    void foundCaptureProperties(QVector<UVCCaptureProperties>);

private:
    //< this function is executed whenever the capture grabs a new frame
    static void callback(uvc_frame_t *frame, void *ptr);

    //< the libuvc service context
    uvc_context_t *m_context = nullptr;

    //< a pointer at a UVC device
    uvc_device_t *m_dev = nullptr;

    //< pointer to an open device. valid only when device is open
    uvc_device_handle_t *m_devh = nullptr;

    //<
    uvc_stream_ctrl_t m_stream_ctrl;

    //<
    uvc_error_t m_error = UVC_SUCCESS; // 0

    //<
    UVCCaptureProperties m_properties;


};

Q_DECLARE_METATYPE(cv::Mat)
Q_DECLARE_METATYPE(UVCCapture::UVCCaptureProperties)

#endif // UVCCAPTURE_H
