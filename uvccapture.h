#ifndef UVCCAPTURE_H
#define UVCCAPTURE_H

#include <QObject>
#include "libuvc/libuvc.h"
#include "uvccapturecontrols.h"
#include <opencv2/opencv.hpp>
#include <QSemaphore>
#include <QVector>
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
        unsigned int bufferSize;
        UVCCaptureProperties():
            format(UVC_FRAME_FORMAT_YUYV),
            formatString("YUYV"),
            frameRect(QRect(0,0,640,480)),
            frameInterval(333333),
            bufferSize(0){}
    };


public:
    UVCCapture(int buffer_size, QSemaphore *free_frames,
               QSemaphore *used_frames, QObject *parent = nullptr);
    ~UVCCapture();

    UVCCaptureControls* controls() { return m_controls; }
    int initUVC();
    void closeUVC();

    uvc_device_handle_t** handlePtr() {return &m_devh;}

    uvc_error negotiateStream();

public slots:
    void findDevices();
    int openDevice(int vid = 0, int pid = 0, QString serial_number = QString());
    void checkDeviceCapabilities(uvc_device_handle_t *devh);
    void setCaptureProperties(UVCCaptureProperties properties);
    void setCaptureActive(bool);
    void setup();

signals:

    void frameAvailable(const cv::Mat &source, const int frame_number);

    //< only announce frame size changes when the frame has changed
    void frameRectChanged(const QRect &rect);

    void foundDevices(QVector<UVCCaptureDescriptor> devices);

    void foundCaptureProperties(QVector<UVCCaptureProperties>);

    void statusMessage(QString message);

    //< for cleanup when thread is finished
    void finished() const;


private:

    UVCCaptureControls* m_controls = nullptr;

    //< pair of semaphores to synchronize capture and cv worker threads
    QSemaphore* m_cap_buffer_free;
    QSemaphore* m_cap_buffer_used;

    //< this function is executed whenever the capture grabs a new frame
    static void callback(uvc_frame_t *frame, void *ptr);

    //< matrix to store the current frame
    QVector<cv::Mat> m_frame_buffer;

    //< the next available index of frame buffer
    int m_next_index = 0;

    int m_dropped_frames = 0;

    //< the libuvc service context
    uvc_context_t *m_context = nullptr;

    //< a pointer at a UVC device
    uvc_device_t *m_dev = nullptr;

    //< pointer to an open device. valid only when device is open
    uvc_device_handle_t *m_devh = nullptr;

    //<
    uvc_stream_ctrl_t m_stream_ctrl;

    //<
    uvc_error_t m_error = UVC_SUCCESS; // UVC_SUCCESS = 0

    //<
    UVCCaptureProperties m_properties;

    //<
    bool m_streaming_active = false;

    //<
    bool m_uvc_open = false;

    bool startStream();
    void stopStream();

    void handleFrame(const cv::Mat &frame, int frame_number);
    void getAllCurrentControls();
    void testControlInfo(uint8_t ctrl);
    void test();

};

Q_DECLARE_METATYPE(cv::Mat)
Q_DECLARE_METATYPE(UVCCapture::UVCCaptureProperties)
Q_DECLARE_METATYPE(UVCCapture::UVCCaptureDescriptor)

#endif // UVCCAPTURE_H
