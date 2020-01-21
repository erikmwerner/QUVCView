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
    m_callback2(callback2), m_cb2_type(cb_2_type), m_handle(handle)

{
    ui->setupUi(this);
    setObjectName(name);

    // put a checkbox in the Ui
    QCheckBox *cb = new QCheckBox(name, this);
    ui->gridLayout->addWidget(cb,0,0);
    connect(cb, &QCheckBox::toggled,
            this,&UVCControlWidget::onCheckBoxToggled);
}

UVCControlWidget::UVCControlWidget(func_uvc callback1, int cb_1_type,
                                   uvc_device_handle_t **handle,
                                   QString name, QString suffix, QWidget *parent) :
    QWidget(parent), ui(new Ui::UVCControlWidget), m_callback1(callback1), m_cb1_type(cb_1_type),
    m_handle(handle)

{
    ui->setupUi(this);

    // put a label on the ui
    ui->gridLayout->addWidget(new QLabel(name, this),0,0);
    if(!suffix.isEmpty()) {
        ui->spinBox->setSuffix(suffix);
    }

    if(!suffix.isEmpty()) {
        ui->spinBox->setSuffix(suffix);
    }
}

UVCControlWidget::~UVCControlWidget()
{
    delete ui;
}

void UVCControlWidget::setRange(int min, int max, int step)
{
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
    QSignalBlocker(ui->spinBox);
    ui->spinBox->setValue(value);
    sendToCallback(m_callback1, m_cb1_type, value);
}

void UVCControlWidget::on_spinBox_valueChanged(int arg1)
{
    QSignalBlocker(ui->horizontalSlider);
    ui->horizontalSlider->setValue(arg1);
    sendToCallback(m_callback1, m_cb1_type, arg1);
}

void UVCControlWidget::sendToCallback(func_uvc callback, int cb_typ, int value)
{
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
