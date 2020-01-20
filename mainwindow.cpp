#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <stdio.h>
#include <QThread>
#include <QTimer>

MainWindow* handler;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _settings_widget = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, _settings_widget);

    m_capture_timer = new QTimer(this);
    m_capture_timer->setTimerType(Qt::PreciseTimer);
    connect(m_capture_timer, &QTimer::timeout,
            this, &MainWindow::grabFrame);

    initUVC();
    handler = this;
}

MainWindow::~MainWindow()
{
    UVCClose();
    delete ui;
}

void MainWindow::handleFrame(uvc_frame_t *bgr)
{
    //qDebug()<<"handling frame";
    //QImage img((uchar*)bgr->data,bgr->width,bgr->height,bgr->step,QImage::Format_RGB888);


    m_frame = cv::Mat(bgr->height, bgr->width,CV_8UC3, (uchar*)bgr->data, bgr->step);

    ui->label->setPixmap(cvMatToQPixmap(m_frame));
}


/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
void cb(uvc_frame_t *frame, void *ptr) {
    //qDebug()<<"got frame";
    uvc_frame_t *bgr;
    uvc_error_t ret;
    /* We'll convert the image from YUV/JPEG to BGR, so allocate space */
    bgr = uvc_allocate_frame(frame->width * frame->height * 3);
    if (!bgr) {
        printf("unable to allocate bgr frame!");
        return;
    }
    /* Do the BGR conversion */
    ret = uvc_any2bgr(frame, bgr);
    if (ret) {
        uvc_perror(ret, "uvc_any2bgr");
        uvc_free_frame(bgr);
        return;
    }

    handler->handleFrame(bgr);
    uvc_free_frame(bgr);
}

void MainWindow::grabFrame()
{
    uvc_frame_t *frame;
    uvc_frame_t *bgr;
    uvc_error_t ret;
    ret = uvc_stream_get_frame(strmhp, &frame, 1000000); //1 s timeout
    if(ret < 0) {
        qDebug()<<"error grabbing frame"<<ret;
    }
    else {
        ret = uvc_any2bgr(frame, bgr);
        QImage img((uchar*)bgr->data,bgr->width,bgr->height,bgr->step,QImage::Format_RGB888);
        ui->label->setPixmap(QPixmap::fromImage(img));
    }
}



int MainWindow::initUVC() {

    /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
    res = uvc_init(&ctx, nullptr);
    if (res < 0) {
        uvc_perror(res, "uvc_init");
        return res;
    }
    qDebug()<<"UVC initialized";

    uvc_device_t ** list;
    res = uvc_get_device_list(ctx, &list);
    if(res < 0 ) {
        qDebug()<<"error listing devices";
    }

    /* Locates the first attached UVC device, stores in dev */
    res = uvc_find_device(ctx, &dev,0, 0, nullptr); /* filter devices: vendor_id, product_id, "serial_num" */
    if (res < 0) {
        qDebug()<<"no device found";
        uvc_perror(res, "uvc_find_device"); /* no devices found */
    }
    else
    {
        qDebug()<<"Device found";

        /* Try to open the device: requires exclusive access */
        res = uvc_open(dev, &devh);
        if (res < 0) {
            uvc_perror(res, "uvc_open"); /* unable to open device */
        }
        else
        {
            qDebug()<<"Device opened";
            /* Print out a message containing all the information that libuvc knows about the device */
            uvc_print_diag(devh, stderr);

            /* Try to negotiate a 640x480 30 fps YUYV stream profile */
            qDebug()<<"Negotiating stream";
            res = uvc_get_stream_ctrl_format_size(
                        devh, &ctrl, /* result stored in ctrl */
                        UVC_FRAME_FORMAT_YUYV, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
                        640, 480, 30 /* width, height, fps */
                        );

            /* Print out the result */
            uvc_print_stream_ctrl(&ctrl, stderr);

            qDebug()<<"Done...";
            if (res < 0) {
                uvc_perror(res, "get_mode"); /* device doesn't provide a matching stream */
            }
            else {
                /* Start the video stream. The library will call user function cb:
                *  cb(frame, (void*) 12345)
                */
                qDebug()<<"Opening stream ctrl...";
                res = uvc_start_streaming(devh, &ctrl, cb, (void*)12345, 0);



                //uvc_stream_open_ctrl(devh, &strmhp, &ctrl);

                //uvc_stream_start(strmh,)
                //qDebug()<<"starting timer with interval:"<<1000/30;
                //m_capture_timer->start(1000/30);
                //uvc_set_ae_mode(devh, 1); /* e.g., turn on auto exposure */
            }

        }
    }
}

void MainWindow::UVCClose()
{
    /* End the stream. Blocks until last callback is serviced */
    uvc_stop_streaming(devh);
    puts("Done streaming.");
    /* Release our handle on the device */
    uvc_close(devh);
    puts("Device closed");
    /* Release the device descriptor */
    uvc_unref_device(dev);
    /* Close the UVC context. This closes and cleans up any existing device handles,
* and it closes the libusb context if one was not provided. */
    uvc_exit(ctx);
    puts("UVC exited");
}
