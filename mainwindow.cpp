#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uvccapturesettings.h"
#include "cameraview.h"
#include "uvccapture.h"
#include "videowriterwidget.h"
#include "fpstimer.h"

#include <opencv2/videoio.hpp>
#include <QLabel>
#include <QToolButton>
#include <QDockWidget>
#include <stdio.h>
#include <QSettings>
#include <QFileDialog>
#include <QSemaphore>
#include <QMessageBox>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("UVC View");

    m_cap_buffer_free = new QSemaphore(m_frame_buffer_size);
    m_cap_buffer_used = new QSemaphore();


    // setup capture device
    m_capture = new UVCCapture(m_frame_buffer_size,
                               m_cap_buffer_free,
                               m_cap_buffer_used,
                               nullptr);

    // make a thread for the capture
    m_capture_thread = new QThread(this);
    m_capture->moveToThread(m_capture_thread);

    // connect thread stuff
    connect(m_capture_thread, &QThread::started,
            m_capture, &UVCCapture::setup);
    connect(m_capture_thread, &QThread::finished,
            m_capture, &UVCCapture::deleteLater);
    connect(this, &QMainWindow::destroyed,
            m_capture_thread, &QThread::quit);
    connect(m_capture, &UVCCapture::finished,
            m_capture_thread, &QThread::quit);
    connect(m_capture, &UVCCapture::finished,
            m_capture, &UVCCapture::deleteLater);

    connect(m_capture, &UVCCapture::frameAvailable,
            this, &MainWindow::handleFrame);
    connect(m_capture, &UVCCapture::frameRectChanged,
            ui->cameraView, &CameraView::onFrameRectChanged);
    connect(m_capture, &UVCCapture::capturePropertiesChanged,
            this, &MainWindow::onCapturePropertiesChanged);
    connect(m_capture, &UVCCapture::statusMessage,
            this, &MainWindow::onCaptureStatusMessage);
    connect(this, &MainWindow::findDevices,
            m_capture, &UVCCapture::findDevices);
    connect(this, &MainWindow::setCaptureActive,
            m_capture, &UVCCapture::setCaptureActive);


     m_capture_thread->start(QThread::NormalPriority);


    // add FPS counter
    m_fps_label = new QLabel(this);
    m_fps_timer = new FPSTimer(this);
    connect(m_fps_timer, &FPSTimer::fpsUpdated,
            this, &MainWindow::onFPSUpdated);
    ui->statusbar->addPermanentWidget(m_fps_label);

    // add toolbuttons to control zoom
    m_button_zoom_in = new QToolButton(this);
    connect(m_button_zoom_in, &QToolButton::clicked,
            this, &MainWindow::onToolButtonZoomInClicked);
    m_button_zoom_out = new QToolButton(this);
    connect(m_button_zoom_out, &QToolButton::clicked,
            this, &MainWindow::onToolButtonZoomOutClicked);
    m_button_zoom_reset = new QToolButton(this);
    connect(m_button_zoom_reset, &QToolButton::clicked,
            this, &MainWindow::onToolButtonZoomResetClicked);
    m_button_zoom_in->setIcon(QIcon(":/icons/zoom-in.png"));
    m_button_zoom_out->setIcon(QIcon(":/icons/zoom-out.png"));
    m_button_zoom_reset->setIcon(QIcon(":/icons/zoom-reset.png"));
    ui->statusbar->addPermanentWidget(m_button_zoom_in);
    ui->statusbar->addPermanentWidget(m_button_zoom_out);
    ui->statusbar->addPermanentWidget(m_button_zoom_reset);

    m_settings_widget = new QDockWidget(this);
    m_settings_widget->setFeatures(
                QDockWidget::DockWidgetMovable |
                QDockWidget::DockWidgetFloatable);

    UVCCaptureSettings* capture_settings = new UVCCaptureSettings(m_capture, this);
    m_settings_widget->setWidget(capture_settings);
    m_settings_widget->setWindowTitle("Capture Settings");
    connect(capture_settings, &UVCCaptureSettings::setCaptureActive,
            this, &MainWindow::onSetCaptureActive);
    addDockWidget(Qt::LeftDockWidgetArea, m_settings_widget);

    m_writer_widget = new QDockWidget(this);
    m_writer_widget->setFeatures(
                QDockWidget::DockWidgetMovable |
                QDockWidget::DockWidgetFloatable);
    VideoWriterWidget* writer = new VideoWriterWidget(this);
    m_writer_widget->setWidget(writer);
    m_writer_widget->setWindowTitle("Video Writer");
    connect(writer, &VideoWriterWidget::saveFrame,
            this, &MainWindow::onSaveCurrentFrame);
    connect(writer, &VideoWriterWidget::startVideoRecording,
            this, &MainWindow::onStartVideoRecording);
    connect(writer, &VideoWriterWidget::stopVideoRecording,
            this, &MainWindow::onStopVideoRecording);

     addDockWidget(Qt::LeftDockWidgetArea, m_writer_widget);

     connect(ui->actionAbout, &QAction::triggered,
             this, &MainWindow::showAbout);

    emit findDevices();
}

MainWindow::~MainWindow()
{
    emit setCaptureActive(false);

    // drain anything in the capture buffer
    while(m_cap_buffer_used->available() > 0){
        m_cap_buffer_used->acquire();
        m_cap_buffer_free->release();
        this->thread()->msleep(50);
        qDebug()<<"capture buffer draining...";
    }
    // let the capture thread shutdown
    if(m_capture_thread != nullptr) {
        m_capture_thread->quit();
        m_capture_thread->wait();
    }

    if(m_recording) {
        onStopVideoRecording();
    }

    delete ui;
}

void MainWindow::showAbout()
{
    QString title(tr("About UVC View"));
    QString text(tr("UVC View Version 0.1"));
    QString info_text(tr("Written 2020\nby Erik Werner\nusing Qt 5.14, libusb 0.0.6, and openCV 4.2."));


    //QMessageBox::about(this, title, text);

    QMessageBox message_box;
    message_box.setWindowTitle(title);
    message_box.setText(text);
    message_box.setInformativeText(info_text);
            message_box.addButton(QMessageBox::Ok);

    message_box.exec();
}

void MainWindow::onCapturePropertiesChanged(UVCCapture::UVCCaptureProperties properties)
{
    m_properties_copy = properties;
}

void MainWindow::onSetCaptureActive(bool active)
{
    m_fps_timer->setRunning(active);
    emit setCaptureActive(active);
}

void MainWindow::handleFrame(const cv::Mat &frame, const int frame_number)
{
    // block until the capture has emitted a frame
    if(!m_cap_buffer_used->tryAcquire()){
        qDebug()<<"warning: no frame to acquire";
        return;
    }
    cv::cvtColor(frame, m_frame,cv::COLOR_BGR2RGB, 3);
    m_pix = cvMatToQPixmap(m_frame);
    ui->cameraView->showFrame(m_pix);
    m_fps_timer->incrementFrame();

    if(m_recording) {
        m_writer.write(m_frame);
    }
    // tell the capture the buffer has room for another frame
    m_cap_buffer_free->release();
}

void MainWindow::onSaveCurrentFrame(const QString &file_name)
{
    if(m_pix.isNull()) {
        return;
    }

    if(m_pix.save(file_name, 0, -1))
    {
        ui->statusbar->showMessage("Saved image:"+file_name);
    }
}

void MainWindow::onStartVideoRecording(const QString &file_name)
{
    int fcc = cv::VideoWriter::fourcc('H','2','6','4');
    cv::Size s(m_properties_copy.frameRect.width(), m_properties_copy.frameRect.height());
    int fps = 10000000.0/m_properties_copy.frameInterval;
    if(m_writer.open(file_name.toStdString(), fcc, fps, s)){
        ui->statusbar->showMessage("Recording video:"+file_name);
        m_recording = true;
    }
    else {
        ui->statusbar->showMessage("Unable to open video recorder:"+file_name);
    }
}

void MainWindow::onStopVideoRecording()
{
    m_writer.release();
    ui->statusbar->showMessage("Video recording stopped");
    m_recording = false;
}

void MainWindow::onCaptureStatusMessage(const QString &message)
{
    ui->statusbar->showMessage(message);
}

void MainWindow::onFPSUpdated(double fps)
{
    m_fps_label->setText(QString::number(fps,'f',2) + " (FPS)");
}

void MainWindow::onToolButtonZoomInClicked()
{
    ui->cameraView->scaleByFactor(1.0/0.75);
}

void MainWindow::onToolButtonZoomOutClicked()
{
    ui->cameraView->scaleByFactor(0.75);
}

void MainWindow::onToolButtonZoomResetClicked()
{
    ui->cameraView->setScaleFactor(1.0);
}
