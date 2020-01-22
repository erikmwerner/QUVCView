#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uvccapturesettings.h"
#include "cameraview.h"
#include "uvccapture.h"
#include "videowriterwidget.h"
#include "fpstimer.h"

#include <QLabel>
#include <QToolButton>
#include <QDockWidget>
#include <stdio.h>
#include <QSettings>
#include <QFileDialog>
#include <QSemaphore>
#include <QThread>

//#include <QOpenGLWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("UVC View");

    m_cap_buffer_free = new QSemaphore(m_frame_buffer_size);
    m_cap_buffer_used = new QSemaphore();


    // setup capture device
    m_capture = new UVCCapture(m_frame_buffer_size, m_cap_buffer_free, m_cap_buffer_used, nullptr);

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

     addDockWidget(Qt::LeftDockWidgetArea, m_writer_widget);

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

    delete ui;
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
    m_frame = frame;
    m_pix =  cvMatToQPixmap(m_frame);
    ui->cameraView->showFrame(m_pix);
    m_fps_timer->incrementFrame();
    // tell the capture the buffer has room for another frame
    m_cap_buffer_free->release();
}

void MainWindow::onSaveCurrentFrame()
{
    QSettings settings;
    QString dir = settings.value("lastImageSaveLocation").toString();
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Frame"),
                               dir, tr("Images (*.png *.xpm *.jpg)"));
    if(!file_name.isEmpty()) {
        m_pix.save(file_name, 0, -1);
        settings.setValue("lastImageSaveLocation", file_name);
    }
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
