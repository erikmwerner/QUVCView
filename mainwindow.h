#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "libuvc/libuvc.h"
#include <opencv2/opencv.hpp>

#include <QDebug>

typedef enum	{
    UVC_AUTO_EXPOSURE_MODE_UNDEFINED = 0x00,	///	undefined auto exposure mode
    UVC_AUTO_EXPOSURE_MODE_MANUAL = 0x01,	///	manual exposure, manual iris
    UVC_AUTO_EXPOSURE_MODE_AUTO = 0x02,	///	auto exposure, auto iris
    UVC_AUTO_EXPOSURE_MODE_SHUTTER_PRIORITY = 0x04,	///	manual exposure, auto iris
    UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY = 0x08	///	auto exposure, manual iris
} UVC_AEMode;

/*	IMPORTANT: ALL THESE DEFINES WERE TAKEN FROM THE USB SPECIFICATION:
    http://www.usb.org/developers/docs/devclass_docs/USB_Video_Class_1_1_090711.zip			*/

#define UVC_CONTROL_INTERFACE_CLASS 0x0E
#define UVC_CONTROL_INTERFACE_SUBCLASS 0x01

//	video class-specific request codes
#define UVC_SET_CUR	0x01
#define UVC_GET_CUR	0x81
#define UVC_GET_MIN	0x82
#define UVC_GET_MAX	0x83
#define UVC_GET_RES 0x84
#define UVC_GET_LEN 0x85
#define UVC_GET_INFO 0x86
#define UVC_GET_DEF 0x87


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QDockWidget;
class QGraphicsPixmapItem;
class CameraView;
class UVCCapture;
class FPSTimer;
class QLabel;
class QToolButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleFrame(const cv::Mat &frame, const int frame_number);
    void onSaveCurrentFrame();
    void onCaptureStatusMessage(const QString &message);
    void onFPSUpdated(double fps);
    void onToolButtonZoomInClicked();
    void onToolButtonZoomOutClicked();
    void onToolButtonZoomResetClicked();
private:
    Ui::MainWindow *ui;
    QDockWidget* m_settings_widget = nullptr;
    QDockWidget* m_writer_widget = nullptr;
    UVCCapture* m_capture = nullptr;
    FPSTimer* m_fps_timer = nullptr;
    QLabel* m_fps_label = nullptr;
    QToolButton* m_button_zoom_in = nullptr;
    QToolButton* m_button_zoom_out = nullptr;
    QToolButton* m_button_zoom_reset = nullptr;
    QPixmap m_pix;
    cv::Mat m_frame;
};

static inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

        return image;
    }

        // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

        return image.rgbSwapped();
    }

        // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }

        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

        image.setColorTable( sColorTable );

        return image;
    }

    default:
        qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}

static inline QPixmap cvMatToQPixmap( const cv::Mat &inMat )
{
    return QPixmap::fromImage( cvMatToQImage( inMat ) );
}

static inline cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true )
{
    switch ( inImage.format() )
    {
    // 8-bit, 4 channel
    case QImage::Format_RGB32:
    {
        cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

        return (inCloneImageData ? mat.clone() : mat);
    }

        // 8-bit, 3 channel
    case QImage::Format_RGB888:
    {
        if ( !inCloneImageData )
            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

        QImage   swapped = inImage.rgbSwapped();

        return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
    }

        // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

        return (inCloneImageData ? mat.clone() : mat);
    }

    default:
        qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
        break;
    }

    return cv::Mat();
}

// If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
// with the cv::Mat directly
//    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
static inline cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true )
{
    return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
}

#endif // MAINWINDOW_H
