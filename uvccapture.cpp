#include "uvccapture.h"
//#include "utlist.h"
#include "libuvc/libuvc.h"
#include <QDebug>

UVCCapture::UVCCapture(QObject *parent) : QObject(parent)
{
    m_controls = new UVCCaptureControls(this, &m_devh);

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

/*!
 * \brief UVCCapture::initUVC start a new UVC service context
 * and sets the m_uvc_open flag to true on success
 * \return
 */
int UVCCapture::initUVC()
{
    if(m_uvc_open) {
        qDebug()<<"UVC already open";
        m_uvc_open = true;
        return 0;
    }
    /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
    m_error = uvc_init(&m_context, nullptr);
    if (m_error) {
        qDebug()<<"UVC could not initialize";
        uvc_perror(m_error, "uvc_init");
        m_uvc_open = false;
        return m_error;
    }
    m_uvc_open = true;
    qDebug()<<"UVC initialized";
    return 0;
}

/*!
 * \brief UVCCapture::findDevices uses the active uvc context
 * to detect devices connected and gather information about them.
 *
 * This function expects a valid uvc service context
 */
void UVCCapture::findDevices()
{
    if(m_context == nullptr) {
        qDebug()<<"Warning: no UVC service context";
        return;
    }
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
 * \brief UVCCapture::checkDeviceCapabilities gathers the video
 * formats supported by a device. This function requires a valid
 * device handle
 * \param devh a handle to an open video device
 */
void UVCCapture::checkDeviceCapabilities(uvc_device_handle_t *devh)
{
    if(devh == nullptr) {
        qDebug()<<"warning: device handle not open";
        return;
    }
    const uvc_format_desc_t* formats = uvc_get_format_descs(devh);

    QVector<UVCCaptureProperties> supported_formats;
    const uvc_format_desc_t *fmt_desc;
    for(fmt_desc = formats; fmt_desc; fmt_desc = fmt_desc->next) {
        UVCCaptureProperties cap_props;
        if(fmt_desc->bDescriptorSubtype == UVC_VS_FORMAT_UNCOMPRESSED){
            cap_props.format = UVC_FRAME_FORMAT_YUYV;
            cap_props.formatString = "YUYV";

        }
        else if(fmt_desc->bDescriptorSubtype == UVC_VS_FORMAT_MJPEG){
            cap_props.format = UVC_FRAME_FORMAT_COMPRESSED;
            cap_props.formatString = "MJPEG";
        }
        else if(fmt_desc->bDescriptorSubtype == UVC_VS_FORMAT_FRAME_BASED){
            cap_props.format = UVC_FRAME_FORMAT_ANY;
            cap_props.formatString = "ANY";
        }
        else {
            cap_props.format = UVC_FRAME_FORMAT_UNKNOWN;
            cap_props.formatString = "Unknown";
        }

        //const uvc_frame_desc* frame_descs = fmt_desc->frame_descs;
        const uvc_frame_desc* frame_desc;
        for(frame_desc = fmt_desc->frame_descs; frame_desc; frame_desc = frame_desc->next) {

            cap_props.frameRect = QRect(0,0,frame_desc->wWidth, frame_desc->wHeight);
            cap_props.frameInterval = frame_desc->dwDefaultFrameInterval;
            cap_props.bufferSize = frame_desc->dwMaxVideoFrameBufferSize;
            supported_formats.append(cap_props);
            /*
            uint32_t *interval_ptr;
            if (frame_desc->intervals) {
                for (interval_ptr = frame_desc->intervals; *interval_ptr; ++interval_ptr) {
                    qDebug()<<"interval"<<(int) (interval_ptr - frame_desc->intervals)<<10000000 / *interval_ptr;
                }
            }*/
        }
    }
    emit foundCaptureProperties(supported_formats);

    /*const uvc_extension_unit_t *ext = uvc_get_extension_units(m_devh);
    uint8_t unitID;
    for(; ext; ext = ext->next) {
        qDebug()<<"ext unit id:"<<ext->bUnitID <<"controls:"<<ext->bmControls;
        unitID = ext->bUnitID;
        getControlInfo(unitID, UVC_CT_AE_MODE_CONTROL); // 2
        getControlInfo(unitID, UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL); // 4
        getControlInfo(unitID, UVC_CT_FOCUS_AUTO_CONTROL); // 8
        getControlInfo(unitID, UVC_CT_FOCUS_ABSOLUTE_CONTROL); // 6
     }*/
}

void UVCCapture::testControlInfo(uint8_t ctrl)
{
    int unitID = uvc_get_processing_units(m_devh)->bUnitID << 8 | m_stream_ctrl.bInterfaceNumber;
    int length = uvc_get_ctrl_len(m_devh, unitID, ctrl);

    qDebug()<<"control:"<<ctrl<<"length"<<length;
    if(length < 0) {
        qDebug()<<"control not supported";
        return;
    }

    uint8_t cmdbuf[length];
    memset(cmdbuf, 0, sizeof(cmdbuf));
    uvc_req_code code = UVC_GET_MIN;
    int response = uvc_get_ctrl(m_devh, unitID, ctrl, cmdbuf, length, code);

    qDebug()<<"min response:"<<response;
    for(int i = 0; i<response; ++i) {
        qDebug()<<"buf["<<i<<"]"<<cmdbuf[i];
    }
    memset(cmdbuf, 0, sizeof(cmdbuf));
    code = UVC_GET_MAX;
    response = uvc_get_ctrl(m_devh, unitID, ctrl, cmdbuf, length, code);

    qDebug()<<"max response:"<<response;
    for(int i = 0; i<response; ++i) {
        qDebug()<<"buf["<<i<<"]"<<cmdbuf[i];
    }
    memset(cmdbuf, 0, sizeof(cmdbuf));
    code = UVC_GET_CUR;
    response = uvc_get_ctrl(m_devh, unitID, ctrl, cmdbuf, length, code);

    qDebug()<<"cur response:"<<response;
    for(int i = 0; i<response; ++i) {
        qDebug()<<"buf["<<i<<"]"<<cmdbuf[i];
    }
    qDebug()<<"--------";
}

void UVCCapture::getAllCurrentControls()
{
    m_controls->getExposureMode();
    m_controls->getAbsExposure();
    m_controls->getFocusMode();
    m_controls->getAbsoluteFocus();
    m_controls->getBackLightCompensation();
    m_controls->getBrightness();
    m_controls->getContrastMode();
    m_controls->getContrast();
    m_controls->getHueMode();
    m_controls->getHue();
    m_controls->getSaturation();
    m_controls->getSharpness();
    m_controls->getGamma();
    m_controls->getWhiteBalanceMode();
    m_controls->getWhiteBalanceTemperature();
}

void UVCCapture::test()
{
    qDebug()<<"--------testing AF--------";
    uint8_t mode;
    uvc_get_focus_auto(m_devh, &mode, UVC_GET_CUR);
    qDebug()<< "Current status (mode):"<<mode;

    qDebug()<<"turning on...";
    uvc_set_focus_auto(m_devh, 1); /* e.g., turn on auto exposure */

    uvc_get_focus_auto(m_devh, &mode, UVC_GET_CUR);
    qDebug()<< "Current status (mode):"<<mode;

    qDebug()<<"turning off...";
    uvc_set_focus_auto(m_devh, 0);

    uvc_get_focus_auto(m_devh, &mode, UVC_GET_CUR);
    qDebug()<< "Current status (mode):"<<mode;
}

/*!
 * \brief UVCCapture::closeUVC closes the active UVC context
 */
void UVCCapture::closeUVC()
{
    // only close if uvc is open
    if(m_uvc_open) {

        // stop stream if applicable
        stopStream();

        // the device handle is valid only if the device is open
        if(m_devh != nullptr)
        {
            uvc_close(m_devh);
            m_devh = nullptr;
            qDebug()<<"closed device";
        }
        else {
            qDebug()<<"device alread closed";
            return;
        }

        /* Release the device descriptor */
        uvc_unref_device(m_dev);

        /* Close the UVC context. This closes and cleans up any existing device handles,
        * and it closes the libusb context if one was not provided. */
        uvc_exit(m_context);

        m_uvc_open = false;
        qDebug()<<"UVC closed";
    }
    else {
        qDebug()<<"UVC alread closed";
    }
}

void UVCCapture::setCaptureProperties(UVCCapture::UVCCaptureProperties properties)
{
    m_properties = properties;
}

/*!
 * \brief UVCCapture::openDevice attempts to open a device handle
 * this function requires a valid UVC context
 * \param vid
 * \param pid
 * \param serial_number
 * \return
 */
int UVCCapture::openDevice(int vid, int pid, QString serial_number)
{
    // if UVC is already open, close and re-open
    if(m_uvc_open) {
        // attempt to close first
        closeUVC();
    }

    // uvc should always be closed here, so init
    initUVC();

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
        //m_controls->setHandle(m_devh);
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


/*!
 * \brief UVCCapture::negotiateStream attempts to set
 * stream properties to the properties currently stored
 * in m_properties
 * \return 0 on success
 */
uvc_error UVCCapture::negotiateStream()
{
    // stop stream if one is active
    stopStream();

    // attempt to set format
    m_error = uvc_get_stream_ctrl_format_size(
                m_devh, &m_stream_ctrl, // result stored in ctrl
                m_properties.format, //YUV 422, aka YUV 4:2:2. try _COMPRESSED
                m_properties.frameRect.width(),
                m_properties.frameRect.height(),
                10000000.0/m_properties.frameInterval // width, height, fps
                );

    if (m_error < 0) {
        // device doesn't provide a matching stream
        uvc_perror(m_error, "get_mode");
    }
    else {
        // Print out the result
        // uvc_print_stream_ctrl(&m_stream_ctrl, stderr);
    }

    return m_error; // returns 0 on success
}

/*!
 * \brief UVCCapture::startStream
 */
bool UVCCapture::startStream()
{
    if(!m_uvc_open) {
        qDebug()<<"device not open";
        m_streaming_active = false;
        return false;
    }
    if(negotiateStream() == UVC_SUCCESS) {
        m_error = uvc_start_streaming(m_devh, &m_stream_ctrl, callback, this, 0);
        if(m_error == UVC_SUCCESS) {
            emit frameRectChanged(m_properties.frameRect);
            m_streaming_active = true;
            emit statusMessage("Getting device controls...");
            getAllCurrentControls();
            QString status_message("Streaming from device (");
            status_message += QString::number(m_properties.frameRect.width()) + "x";
            status_message += QString::number(m_properties.frameRect.height()) + ")";
            emit statusMessage(status_message);
            qDebug()<<"UVC streaming started";
            return true;
        }
    }
    m_streaming_active = false;
    return false;
}

void UVCCapture::stopStream()
{
    if(m_streaming_active) {
        // End the stream. Blocks until last callback is serviced
        uvc_stop_streaming(m_devh);
        // set a flag to remember streaming has been stopped
        m_streaming_active = false;
        emit statusMessage("Streaming stopped");
        qDebug()<<"UVC streaming stopped";
    }
    else {
        qDebug()<<"UVC streaming already stopped";
    }

}

/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
void UVCCapture::callback(uvc_frame_t *frame, void *ptr)
{
    // store the result (no access to member variables here)
    uvc_error_t error;

    //allocate RGB frame
    uvc_frame_t *bgr_frame;
    bgr_frame = uvc_allocate_frame(frame->width * frame->height * 3);
    if (!bgr_frame) {
        printf("unable to allocate bgr frame!");
        return;
    }
    cv::Mat cv_frame;

    // if compressed video stream, used mjpeg conversion
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
        // if uncompressed video, use any2bgr
        error = uvc_any2bgr(frame, bgr_frame);
        if (error) {
            qDebug()<<"frame conversion error";
            uvc_free_frame(bgr_frame);
            return;
        }
        else {
            // Example: convert frame to QImage
            //QImage image = QImage((uchar *) rgb_Frame->data, rgb_Frame->width, rgb_Frame->height, QImage::Format_RGB888);
            cv_frame = cv::Mat(bgr_frame->height, bgr_frame->width,CV_8UC3, (uchar*)bgr_frame->data, bgr_frame->step);
        }
    }

    //Get this object object
    UVCCapture *capture = reinterpret_cast<UVCCapture*>(ptr);

    emit capture->frameAvailable(cv_frame, frame->sequence);

    //Free conversion frame memory
    uvc_free_frame(bgr_frame);
}
