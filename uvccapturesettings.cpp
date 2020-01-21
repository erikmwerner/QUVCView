#include "uvccapturesettings.h"
#include "ui_uvccapturesettings.h"
#include "uvccapture.h"

#include "uvccontrolwidget.h"
#include "libuvc/libuvc.h"
#include <QMetaType>
#include <QDebug>

UVCCaptureSettings::UVCCaptureSettings(UVCCapture *parent) :
    QWidget(), ui(new Ui::UVCCaptureSettings), m_parent(parent)
{
    ui->setupUi(this);

    connect(parent, &UVCCapture::foundDevices,
            this,&UVCCaptureSettings::onDevicesFound);
    connect(parent, &UVCCapture::foundCaptureProperties,
            this,&UVCCaptureSettings::onSupportedFormatsFound);
    connect(ui->pushButtonRefresh, &QPushButton::clicked,
            parent,&UVCCapture::findDevices);
    connect(ui->pushButtonConnect, &QPushButton::clicked,
            parent,&UVCCapture::startStream);
    connect(this, &UVCCaptureSettings::openDevice,
            parent, &UVCCapture::openDevice);
    connect(this, &UVCCaptureSettings::capturePropertiesChanged,
            parent, &UVCCapture::setCaptureProperties);

    int rows = ui->gridLayout->rowCount();
    func_uvc cb1, cb2;
    cb1.uvc_cb_u16 = uvc_set_focus_abs;
    cb2.uvc_cb_u8 = uvc_set_focus_auto;
    ui->gridLayout->addWidget(new UVCControlWidget(
                                  cb1, 'u16', cb2, 'u8', 1, 0, m_parent->handlePtr(),
                                  "Auto Focus", " mm", this), rows++,0, 1, 2);

    cb1.uvc_cb_u32 = uvc_set_exposure_abs;
    cb2.uvc_cb_u8 = uvc_set_ae_mode;
    ui->gridLayout->addWidget(new UVCControlWidget(
                                  cb1, 'u32',  cb2, 'u8', 2, 1, m_parent->handlePtr(),
                                  "Auto Exposure", " us", this), rows++,0, 1, 2);

    cb1.uvc_cb_u16 = uvc_set_contrast;
    ui->gridLayout->addWidget(new UVCControlWidget(
                                  cb1, 'u16', m_parent->handlePtr(),
                                  "Contrast", "", this), rows++,0, 1, 2);

    cb1.uvc_cb_16 = uvc_set_brightness;
    ui->gridLayout->addWidget(new UVCControlWidget(
                                  cb1, '16', m_parent->handlePtr(),
                                  "Brightness", "", this), rows++,0, 1, 2);
}

UVCCaptureSettings::~UVCCaptureSettings()
{
    delete ui;
}

/*!
 * \brief UVCCaptureSettings::onDevicesFound populate a combobox
 * with information about the current UVC devices connected
 * \param devices
 */
void UVCCaptureSettings::onDevicesFound(QVector<UVCCapture::UVCCaptureDescriptor> devices)
{
    ui->comboBoxDevices->blockSignals(true);
    ui->comboBoxDevices->clear();
    foreach(UVCCapture::UVCCaptureDescriptor device, devices){
        QString name_string = device.productName;
        QString description_string = name_string;
        description_string +=" Vendor ID:";
        description_string +=QString::number(device.vendorID);
        description_string +=" Product ID:";
        description_string +=QString::number(device.productID);
        description_string +=" Serial Number:";
        description_string +=device.serial;
        ui->comboBoxDevices->addItem(name_string);
        int index = ui->comboBoxDevices->count() - 1;
        ui->comboBoxDevices->setItemData(index, QVariant(device.vendorID), Qt::UserRole);
        ui->comboBoxDevices->setItemData(index, QVariant(device.productID), Qt::UserRole + 1);
        ui->comboBoxDevices->setItemData(index, QVariant(device.serial), Qt::UserRole + 2);
        ui->comboBoxDevices->setItemData(index, description_string, Qt::ToolTipRole);
    }

    ui->labelDevice->setText("Found" + QString::number(devices.length()) + " UVC device(s)");
    if(devices.length() == 0) {
        ui->comboBoxDevices->addItem("No devices found");
        return;
    }
    ui->comboBoxDevices->blockSignals(false);

    ui->comboBoxDevices->setCurrentIndex(0);
    on_comboBoxDevices_currentIndexChanged(0);
}

void UVCCaptureSettings::onSupportedFormatsFound(QVector<UVCCapture::UVCCaptureProperties> formats)
{
    ui->comboBoxFormat->clear();
    foreach(UVCCapture::UVCCaptureProperties format, formats) {
        QString format_string = QString::number(format.frameRect.width());
        format_string += "x";
        format_string += QString::number(format.frameRect.height());
        format_string += " ";
        format_string += QString::number(10000000.0 / format.frameInterval);
        format_string += " FPS (";
        format_string +=format.formatString;
        format_string += ")";
        ui->comboBoxFormat->addItem(format_string);
        int index = ui->comboBoxFormat->count() - 1;
        QVariant v;
        v.setValue(format);
        ui->comboBoxFormat->setItemData(index, v, Qt::UserRole);
    }
    if(formats.length() == 0) {
        ui->comboBoxDevices->addItem("Connect device");
        return;
    }

    ui->comboBoxFormat->setCurrentIndex(0);
    on_comboBoxFormat_currentIndexChanged(0);
}

/*!
 * \brief UVCCaptureSettings::on_comboBoxDevices_currentIndexChanged
 * will gather UVC information stored in the current index and attempt to open
 * the device to discover compativble formats
 * \param index
 */
void UVCCaptureSettings::on_comboBoxDevices_currentIndexChanged(int index)
{
    qDebug()<<"cb index changed"<<index<<"text"<<ui->comboBoxDevices->itemText(index);

    int vid = ui->comboBoxDevices->itemData(index, Qt::UserRole).toInt();
    int pid = ui->comboBoxDevices->itemData(index, Qt::UserRole + 1).toInt();
    QString serial = ui->comboBoxDevices->itemData(index, Qt::UserRole + 2).toString();
    if(vid != 0 && pid != 0) {
        emit openDevice(vid, pid, serial);
    }
}

void UVCCaptureSettings::on_comboBoxFormat_currentIndexChanged(int index)
{
    qDebug()<<"cb index changed"<<index<<"text"<<ui->comboBoxFormat->itemText(index);
    QVariant v =  ui->comboBoxFormat->itemData(index,Qt::UserRole);
    UVCCapture::UVCCaptureProperties p = v.value<UVCCapture::UVCCaptureProperties>();
    emit capturePropertiesChanged(p);
}
