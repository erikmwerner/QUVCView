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
#include <QThread>

//#include <QOpenGLWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("UVC View");

    // setup capture device
    m_capture = new UVCCapture(this);
    connect(m_capture, &UVCCapture::frameAvailable,
            this, &MainWindow::handleFrame);
    connect(m_capture, &UVCCapture::frameRectChanged,
            ui->cameraView, &CameraView::onFrameRectChanged);
    connect(m_capture, &UVCCapture::statusMessage,
            this, &MainWindow::onCaptureStatusMessage);

    // add FPS counter
    m_fps_label = new QLabel(this);
    m_fps_timer = new FPSTimer(this);
    m_fps_timer->setRunning(true);
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
    m_fps_timer->incrementFrame();
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
    m_fps_label->setText(QString::number(fps,'g',2) + " (FPS)");
}

void MainWindow::onToolButtonZoomInClicked()
{
    ui->cameraView->setScaleFactor(1.0/0.75);
}

void MainWindow::onToolButtonZoomOutClicked()
{
    ui->cameraView->setScaleFactor(0.75);
}

void MainWindow::onToolButtonZoomResetClicked()
{
    ui->cameraView->setScaleFactor(1.0);
}
