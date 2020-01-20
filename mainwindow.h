#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "libuvc/libuvc.h"

class QDockWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //void callback(uvc_frame_t *frame, void *ptr);

    int initUVC();

private:
    Ui::MainWindow *ui;
    QDockWidget* _settings_widget = nullptr;

    uvc_context_t *ctx;
    uvc_device_t *dev;
    uvc_device_handle_t *devh;
    uvc_stream_ctrl_t ctrl;
    uvc_error_t res;
};
#endif // MAINWINDOW_H
