#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uvccapturesettings.h"
#include "cameraview.h"
#include "uvccapture.h"

#include <QDockWidget>
#include <stdio.h>
#include <QThread>

//#include <QOpenGLWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("UVC View");


    m_capture = new UVCCapture(this);
    connect(m_capture, &UVCCapture::frameAvailable,
            this, &MainWindow::handleFrame);
    connect(m_capture, &UVCCapture::frameRectChanged,
            ui->cameraView, &CameraView::onFrameRectChanged);


    _settings_widget = new QDockWidget(this);
    _settings_widget->setWidget(new UVCCaptureSettings(m_capture));
    _settings_widget->setWindowTitle("Capture Settings");
    addDockWidget(Qt::RightDockWidgetArea, _settings_widget);

    m_capture->initUVC();
    m_capture->findDevices();
    //m_capture->openDevice();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleFrame(const cv::Mat &frame, const int frame_number)
{
    m_frame = frame;
    m_pix =  cvMatToQPixmap(m_frame);
    ui->cameraView->showFrame(m_pix);
}
