#ifndef UVCCONTROLWIDGET_H
#define UVCCONTROLWIDGET_H

#include <QWidget>
#include "libuvc/libuvc.h"

namespace Ui {
class UVCControlWidget;
}

//< use this union to store pointers to uvc functions that
//< take different argument types. ie: use uvc_cv_u8 to
//< store a callback to a uvc function that takes uint8_t
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
    /*!
     * \brief UVCControlWidget oerloaded constructor for controls that
     * have both range and checkable control options
     * \param callback1
     * \param cb_1_type
     * \param callback2
     * \param cb_2_type
     * \param value_checked
     * \param value_unchecked
     * \param handle
     * \param name
     * \param suffix
     * \param parent
     */
    explicit UVCControlWidget(func_uvc callback1, int cb_1_type, func_uvc callback2, int cb_2_type,
                              int value_checked, int value_unchecked,
                              uvc_device_handle_t **handle,
                              QString name, QString suffix, QWidget *parent = nullptr);
    /*!
     * \brief UVCControlWidget overloaded constructor for controls that
     * do not have a checkable option
     * \param callback1
     * \param cb_1_type
     * \param handle
     * \param name
     * \param suffix
     * \param parent
     */
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
