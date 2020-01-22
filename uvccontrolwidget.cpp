#include "uvccontrolwidget.h"
#include "ui_uvccontrolwidget.h"
#include <QCheckBox>
#include <QLabel>
#include <QDebug>

UVCControlWidget::UVCControlWidget(func_uvc callback1, int cb_1_type,
                                   func_uvc callback2, int cb_2_type,
                                   int value_checked, int value_unchecked,
                                   uvc_device_handle_t **handle,
                                   QString name, QString suffix, QWidget *parent) :
    QWidget(parent), ui(new Ui::UVCControlWidget), m_callback1(callback1), m_cb1_type(cb_1_type),
    m_callback2(callback2), m_cb2_type(cb_2_type), m_value_checked(value_checked),
    m_value_unchecked(value_unchecked), m_handle(handle)

{
    ui->setupUi(this);
    setObjectName(name);

    // put a label on the ui
    ui->label->setText(name);
    ui->checkBox->setText("Auto");

    connect(ui->checkBox, &QCheckBox::toggled,
            this,&UVCControlWidget::onCheckBoxToggled);

    if(!suffix.isEmpty()) {
        ui->spinBox->setSuffix(suffix);
    }
}

UVCControlWidget::UVCControlWidget(func_uvc callback1, int cb_1_type,
                                   uvc_device_handle_t **handle,
                                   QString name, QString suffix, QWidget *parent) :
    QWidget(parent), ui(new Ui::UVCControlWidget), m_callback1(callback1), m_cb1_type(cb_1_type),
    m_handle(handle)

{
    ui->setupUi(this);
    setObjectName(name);

    // put a label on the ui
    ui->label->setText(name);

    if(!suffix.isEmpty()) {
        ui->spinBox->setSuffix(suffix);
    }

    ui->checkBox->setVisible(false);
}

UVCControlWidget::~UVCControlWidget()
{
    delete ui;
}

void UVCControlWidget::setRange(int min, int max, int step)
{
    const QSignalBlocker blocker1(ui->spinBox);
    const QSignalBlocker blocker2(ui->horizontalSlider);
    ui->spinBox->setRange(min, max);
    ui->spinBox->setSingleStep(step);

    ui->horizontalSlider->setRange(min, max);
    ui->horizontalSlider->setSingleStep(step);
}

void UVCControlWidget::onCheckBoxToggled(bool checked)
{
    if(checked) {
        ui->horizontalSlider->setEnabled(false);
        ui->spinBox->setEnabled(false);
        sendToCallback(m_callback2, m_cb2_type, m_value_checked);
    }
    else {
        ui->horizontalSlider->setEnabled(true);
        ui->spinBox->setEnabled(true);
        sendToCallback(m_callback2, m_cb2_type, m_value_unchecked);
    }
}

void UVCControlWidget::on_horizontalSlider_valueChanged(int value)
{
    const QSignalBlocker blocker(ui->spinBox);
    ui->spinBox->setValue(value);
    sendToCallback(m_callback1, m_cb1_type, value);
}

void UVCControlWidget::on_spinBox_valueChanged(int arg1)
{
    const QSignalBlocker bloccker(ui->horizontalSlider);
    ui->horizontalSlider->setValue(arg1);
    sendToCallback(m_callback1, m_cb1_type, arg1);
}

void UVCControlWidget::sendToCallback(func_uvc callback, int cb_typ, int value)
{
    if(*m_handle == nullptr) {
        qDebug()<<"Warning: handle is a null pointer"<<objectName();
        return;
    }
    switch(m_cb1_type) {
    case 'u8':
        callback.uvc_cb_u8(*m_handle, value);
        break;
    case 'u16':
        callback.uvc_cb_u16(*m_handle, value);
        break;
    case 'u32':
        callback.uvc_cb_u32(*m_handle, value);
        break;
    case '8':
        callback.uvc_cb_8(*m_handle, value);
        break;
    case '16':
        callback.uvc_cb_16(*m_handle, value);
        break;
    case '32':
        callback.uvc_cb_32(*m_handle, value);
        break;
    }
    qDebug()<<"UVC control widget"<<objectName()<<"sent value"<<value;
}
