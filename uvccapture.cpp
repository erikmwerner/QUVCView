#include "uvccapture.h"
//#include "utlist.h"
#include "libuvc/libuvc.h"
#include <QDebug>

UVCCapture::UVCCapture(QObject *parent) : QObject(parent)
{
    int id = QMetaType::type("cv::Mat");
    if(id == QMetaType::UnknownType) {
        qRegisterMetaType<cv::Mat>("cv::Mat");
    }
    id = QMetaType::type("UVCCaptureProperties");
    if(id == QMetaType::UnknownType) {
        qRegisterMetaType<UVCCaptureProperties>("UVCCaptureProperties");
    }
}

UVCCapture::~UVCCapture()
{
    closeUVC();
}

int UVCCapture::initUVC()
{
    /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
    m_error = uvc_init(&m_context, nullptr);
    if (m_error) {
        qDebug()<<"UVC could not initialize";
        uvc_perror(m_error, "uvc_init");
        return m_error;
    }
    qDebug()<<"UVC initialized";
    return 0;
}

void UVCCapture::findDevices()
{
    //closeUVC();
    //initUVC();
    uvc_device_t **list;
    m_error = uvc_get_device_list(m_context, &list);
    if(m_error < 0 ) {
        qDebug()<<"error listing devices";
        uvc_perror(m_error, "find devices");
    }

    // count the number of devices found
    int count = 0;
    while(list != nullptr && list[count] != nullptr) {
        ++count;
    }
    qDebug()<<"Found"<<count<<"UVC devices";

    QVector<UVCCaptureDescriptor> devices;

    // iterate through found devices and
    for(int k = 0; k<count; ++k) {
        uvc_device_t* device = list[k];
        struct uvc_device_descriptor* desc;
        uvc_get_device_descriptor(device, &desc);
        UVCCaptureDescriptor d;
        uint8_t bus = uvc_get_bus_number(device);
        uint8_t addr = uvc_get_device_address(device);
        d.vendorID = desc->idVendor;
        d.productID = desc->idProduct;
        d.UVC_level = desc->bcdUVC;
        d.serial = QString(desc->serialNumber);
        d.manufacturer = QString(desc->manufacturer);
        d.productName = QString(desc->product);
        d.dev = device;

        devices.append(d);

        uvc_free_device_descriptor(desc);
    }
    // free the device list when finished
    uvc_free_device_list(list, 1);

    emit foundDevices(devices);
}

/*!
 * \brief UVCCapture::checkDeviceCapabilities gets video
 * formats supported by a device
 * \param devh a handle to an open video device
 */
void UVCCapture::checkDeviceCapabilities(uvc_device_handle_t *devh)
{
    if(devh == nullptr) {
        qDebug()<<"warning: handle not open";
        return;
    }
    const uvc_format_desc_t* formats = uvc_get_format_descs(devh);
    qDebug()<<"got formats";

    QVector<UVCCaptureProperties> supported_formats;
    const uvc_format_desc_t *fmt_desc;
    for(fmt_desc = formats; fmt_desc; fmt_desc = fmt_desc->next) {
        UVCCaptureProperties cap_props;
        if(fmt_desc->bDescriptorSubtype == UVC_VS_FORMAT_UNCOMPRESSED){
            cap_props.format = UVC_FRAME_FORMAT_YUYV;
            cap_props.formatString = "YUYV";
            qDebug()<<"uncompressed format";

        }
        else if(fmt_desc->bDescriptorSubtype == UVC_VS_FORMAT_MJPEG){
            cap_props.format = UVC_FRAME_FORMAT_COMPRESSED;
            cap_props.formatString = "MJPEG";
            qDebug()<<"mjpeg format";
        }
        else if(fmt_desc->bDescriptorSubtype == UVC_VS_FORMAT_FRAME_BASED){
            cap_props.format = UVC_FRAME_FORMAT_ANY;
            cap_props.formatString = "ANY";
            qDebug()<<"frame based format";
        }
        else {
            cap_props.format = UVC_FRAME_FORMAT_UNKNOWN;
            cap_props.formatString = "Unknown";
            qDebug()<<"unknown format";
        }

        //const uvc_frame_desc* frame_descs = fmt_desc->frame_descs;
        const uvc_frame_desc* frame_desc;
        for(frame_desc = fmt_desc->frame_descs; frame_desc; frame_desc = frame_desc->next) {

            qDebug()<<"Frame Descriptor:";
            qDebug()<<"Size:"<<frame_desc->wWidth<<frame_desc->wHeight;
            qDebug()<<"Frame rate:"<<frame_desc->dwDefaultFrameInterval;
            qDebug()<<"Buffer size:"<<frame_desc->dwMaxVideoFrameBufferSize;
            cap_props.frameRect = QRect(0,0,frame_desc->wWidth, frame_desc->wHeight);
            cap_props.frameInterval = frame_desc->dwDefaultFrameInterval;
            supported_formats.append(cap_props);

            uint32_t *interval_ptr;
            if (frame_desc->intervals) {
                for (interval_ptr = frame_desc->intervals; *interval_ptr; ++interval_ptr) {
                    qDebug()<<"interval"<<(int) (interval_ptr - frame_desc->intervals)<<10000000 / *interval_ptr;
                }
            }
        }
    }
    emit foundCaptureProperties(supported_formats);
}

void UVCCapture::closeUVC()
{
    /* End the stream. Blocks until last callback is serviced */
    uvc_stop_streaming(m_devh);
    puts("Done streaming.");
    /* Release our handle on the device */
    uvc_close(m_devh);
    puts("Device closed");
    /* Release the device descriptor */
    uvc_unref_device(m_dev);

    /* Close the UVC context. This closes and cleans up any existing device handles,
    * and it closes the libusb context if one was not provided. */
    uvc_exit(m_context);
    puts("UVC exited");
}

void UVCCapture::openDevicePtr(uvc_device_t *dev)
{
    // store this device pointer in a member variable fist
    m_dev = dev;

    // Try to open the device (requires exclusive access)
    // (Device to open, handle to opened device)
    m_error = uvc_open(m_dev, &m_devh);
    if (m_error < 0) {
        // unable to open device
        uvc_perror(m_error, "uvc_open");
    }
    else
    {
        qDebug()<<"Device opened";
        checkDeviceCapabilities(m_devh);
    }
}

void UVCCapture::setCaptureProperties(UVCCapture::UVCCaptureProperties properties)
{
    m_properties = properties;
}

int UVCCapture::openDevice(int vid, int pid, QString serial_number)
{
    closeDevice();

    qDebug()<<"Opening UVC device vid:"<<vid<<"pid"<<pid<<"sn"<<serial_number;
    //m_error = uvc_find_device(m_context, &m_dev,vid, pid, nullptr); /* filter devices: vendor_id, product_id, "serial_num" */
    m_error = uvc_find_device(m_context, &m_dev,vid, pid, serial_number.toUtf8().constData()); /* filter devices: vendor_id, product_id, "serial_num" */
    if (m_error < 0) {
        qDebug()<<"uvc find device: no device found";
        uvc_perror(m_error, "uvc_find_device"); /* no devices found */
        return m_error;
    }
    else
    {
        qDebug()<<"Device found";

        // Try to open the device (requires exclusive access)
        // (Device to open, handle to opened device)
        m_error = uvc_open(m_dev, &m_devh);
        if (m_error < 0) {
            // unable to open device
            uvc_perror(m_error, "uvc_open");
            return m_error;
        }
        else
        {
            qDebug()<<"Device opened";
            qDebug()<<"checking capabilities";
            checkDeviceCapabilities(m_devh);
        }
    }
    return m_error; // no errors, return 0
}

void UVCCapture::closeDevice()
{
    if(m_devh == nullptr)
    {
        return;
    }
    else {
        uvc_close(m_devh);
    }
}

void UVCCapture::startStream()
{
    // Print out a message containing all the information that libuvc knows about a device
    //uvc_print_diag(m_devh, stderr);

    /* Try to negotiate a 640x480 30 fps YUYV stream profile */
    qDebug()<<"Negotiating stream";
    m_error = uvc_get_stream_ctrl_format_size(
                m_devh, &m_stream_ctrl, // result stored in ctrl
                m_properties.format, //YUV 422, aka YUV 4:2:2. try _COMPRESSED
                m_properties.frameRect.width(),
                m_properties.frameRect.height(),
                10000000.0/m_properties.frameInterval // width, height, fps
                );

   /* m_error = uvc_get_stream_ctrl_format_size(
                m_devh, &m_stream_ctrl, // result stored in ctrl
                UVC_FRAME_FORMAT_MJPEG, // YUV 422, aka YUV 4:2:2. try _COMPRESSED
                800,
                600,
                15 // width, height, fps
                );*/


    /* Print out the result */
    uvc_print_stream_ctrl(&m_stream_ctrl, stderr);

    qDebug()<<"Done...";
    if (m_error < 0) {
        uvc_perror(m_error, "get_mode"); /* device doesn't provide a matching stream */

    }
    else {
        /* Start the video stream. The library will call user function cb:
        *  cb(frame, (void*) 12345)
        */
        qDebug()<<"Opening stream ctrl...";
        m_error = uvc_start_streaming(m_devh, &m_stream_ctrl, callback, this, 0);
        emit frameRectChanged(m_properties.frameRect);
        //uvc_stream_open_ctrl(devh, &strmhp, &ctrl);

        //uvc_stream_start(strmh,)
        //qDebug()<<"starting timer with interval:"<<1000/30;
        //m_capture_timer->start(1000/30);
        //uvc_set_ae_mode(devh, 1); /* e.g., turn on auto exposure */
        //uvc_set_exposure_abs(devh, 1000);
    }
}

/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
void UVCCapture::callback(uvc_frame_t *frame, void *ptr)
{
    uvc_frame_t *bgr_frame;
    uvc_error_t error;

    //allocate RGB frame
    bgr_frame = uvc_allocate_frame(frame->width * frame->height * 3);
    if (!bgr_frame) {
        printf("unable to allocate bgr frame!");
        return;
    }
    cv::Mat cv_frame;
    //Convert to RGB
    if(frame->frame_format == UVC_FRAME_FORMAT_MJPEG) {
        error = uvc_mjpeg2rgb(frame, bgr_frame);
        if (error) {
            qDebug()<<"frame conversion error";
            uvc_free_frame(bgr_frame);
            return;
        }
        else {
            cv_frame = cv::Mat(bgr_frame->height, bgr_frame->width,CV_8UC3, (uchar*)bgr_frame->data, bgr_frame->step);
            cv::cvtColor(cv_frame,cv_frame,cv::COLOR_BGR2RGB);
        }
    }
    else {
        error = uvc_any2bgr(frame, bgr_frame);
        if (error) {
            qDebug()<<"frame conversion error";
            uvc_free_frame(bgr_frame);
            return;
        }
        else {
            cv_frame = cv::Mat(bgr_frame->height, bgr_frame->width,CV_8UC3, (uchar*)bgr_frame->data, bgr_frame->step);
        }
    }

    //Get this object object
    UVCCapture *capture = reinterpret_cast<UVCCapture*>(ptr);
    //Convert frame to QImage
    //QImage image = QImage((uchar *) rgb_Frame->data, rgb_Frame->width, rgb_Frame->height, QImage::Format_RGB888);

    emit capture->frameAvailable(cv_frame, frame->sequence);

    //Free conversion frame memory
    uvc_free_frame(bgr_frame);
}
