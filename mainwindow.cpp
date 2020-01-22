#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uvccapturesettings.h"
#include "cameraview.h"
#include "uvccapture.h"
#include "videowriterwidget.h"

#include <QDockWidget>
#include <stdio.h>
#include <QSettings>
#include <QFileDialog>
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


    m_settings_widget = new QDockWidget(this);
    m_settings_widget->setFeatures(
                QDockWidget::DockWidgetMovable |
                QDockWidget::DockWidgetFloatable);

    m_settings_widget->setWidget(new UVCCaptureSettings(m_capture));
    m_settings_widget->setWindowTitle("Capture Settings");
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

    m_capture->initUVC();
    m_capture->findDevices();
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
