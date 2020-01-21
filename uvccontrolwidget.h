#ifndef UVCCONTROLWIDGET_H
#define UVCCONTROLWIDGET_H

#include <QWidget>
#include "libuvc/libuvc.h"

namespace Ui {
class UVCControlWidget;
}

//< store function pointers with different arguments
typedef union {
  uvc_error_t (*uvc_cb_u8)(uvc_device_handle_t*, uint8_t);
  uvc_error_t (*uvc_cb_u16)(uvc_device_handle_t*, uint16_t);
  uvc_error_t (*uvc_cb_u32)(uvc_device_handle_t*, uint32_t);
  uvc_error_t (*uvc_cb_8)(uvc_device_handle_t*, int8_t);
  uvc_error_t (*uvc_cb_16)(uvc_device_handle_t*, int16_t);
  uvc_error_t (*uvc_cb_32)(uvc_device_handle_t*, int32_t);
} func_uvc;

class UVCControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UVCControlWidget(func_uvc callback1, int cb_1_type, func_uvc callback2, int cb_2_type,
                              int value_checked, int value_unchecked,
                              uvc_device_handle_t **handle,
                              QString name, QString suffix, QWidget *parent = nullptr);
    explicit UVCControlWidget(func_uvc callback1, int cb_1_type,
                              uvc_device_handle_t **handle,
                              QString name, QString suffix, QWidget *parent = nullptr);
    ~UVCControlWidget();

    void setRange(int min = 0, int max = 1024, int step = 1);

private slots:
    void onCheckBoxToggled(bool checked);

    void on_horizontalSlider_valueChanged(int value);

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::UVCControlWidget *ui;
    func_uvc m_callback1;
    int m_cb1_type;
    func_uvc m_callback2;
    int m_cb2_type;
    int m_value_checked;
    int m_value_unchecked;

    uvc_device_handle_t** m_handle = nullptr;
    void sendToCallback(func_uvc callback, int cb_typ, int value);
};

#endif // UVCCONTROLWIDGET_H
