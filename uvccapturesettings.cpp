#include "uvccapturesettings.h"
#include "ui_uvccapturesettings.h"
#include "uvccapture.h"

#include "uvccontrolwidget.h"
#include "libuvc/libuvc.h"
#include <QSettings>
#include <QFileDialog>
#include <QMetaType>
#include <QDebug>

UVCCaptureSettings::UVCCaptureSettings(UVCCapture *capture, QWidget* parent) :
    QWidget(parent), ui(new Ui::UVCCaptureSettings)
{
    ui->setupUi(this);

    connect(capture, &UVCCapture::foundDevices,
            this,&UVCCaptureSettings::onDevicesFound);
    connect(capture, &UVCCapture::foundCaptureProperties,
            this,&UVCCaptureSettings::onSupportedFormatsFound);
    connect(ui->pushButtonRefresh, &QPushButton::clicked,
            capture,&UVCCapture::findDevices);
    connect(ui->pushButtonConnect, &QPushButton::clicked,
            this, &UVCCaptureSettings::startCapture);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked,
            this, &UVCCaptureSettings::stopCapture);
    connect(this, &UVCCaptureSettings::openDevice,
            capture, &UVCCapture::openDevice);
    connect(this, &UVCCaptureSettings::capturePropertiesChanged,
            capture, &UVCCapture::setCaptureProperties);

    ////////////////////////
    // add control widgets //
    ////////////////////////
    QVector<QPair<int, QString> >modes;
    int row = 0;
    int col = 0;
    // Exposure
    modes.append(QPair<int, QString>(8,"Auto"));
    modes.append(QPair<int, QString>(1,"Auto"));
    QVector<QString>mode_text;
    UVCControlWidget* w = new UVCControlWidget("Exposure", modes, " us*100", this);
    connect(w, &UVCControlWidget::requestMode,
            capture->controls(), &UVCCaptureControls::getExposureMode);
    connect(w, &UVCControlWidget::modeChanged,
            capture->controls(), &UVCCaptureControls::setExposureMode);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getAbsExposure);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setAbsExposure);
    connect(capture->controls(), &UVCCaptureControls::exposureMode,
            w, &UVCControlWidget::setMode);
    connect(capture->controls(), &UVCCaptureControls::exposure,
            w, &UVCControlWidget::setValue);
    w->setRange(50, 10000);
    w->setDefaultMode(8);
    //ui->groupBox->ver
    ui->gridLayout_2->addWidget(w,row++,col);

    // Focus
    modes.clear();
    modes.append(QPair<int, QString>(1,"Auto"));
    modes.append(QPair<int, QString>(0,"Auto"));
    w = new UVCControlWidget("Focus", modes, " mm", this);
    connect(w, &UVCControlWidget::requestMode,
           capture->controls(), &UVCCaptureControls::getFocusMode);
    connect(w, &UVCControlWidget::modeChanged,
            capture->controls(), &UVCCaptureControls::setFocusMode);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getAbsoluteFocus);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setAbsoluteFocus);
    connect(capture->controls(), &UVCCaptureControls::focusMode,
            w, &UVCControlWidget::setMode);
    connect(capture->controls(), &UVCCaptureControls::absoluteFocus,
            w, &UVCControlWidget::setValue);
    w->setRange(0, 1023);
    w->setDefaultMode(1);
    ui->gridLayout_2->addWidget(w,row++,col);

    // Backlight
    modes.clear();
    w = new UVCControlWidget("Backlight Compensation", modes, "", this);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getBackLightCompensation);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setBackLightCompensation);
    connect(capture->controls(), &UVCCaptureControls::backlightCompensation,
            w, &UVCControlWidget::setValue);
    w->setRange(0, 1);
    w->setDefaultValue(0);
    ui->gridLayout_2->addWidget(w,row++,col);

    // Brightness
    modes.clear();
    w = new UVCControlWidget("Brightness", modes, "", this);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getBrightness);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setBrightness);
    connect(capture->controls(), &UVCCaptureControls::brightness,
            w, &UVCControlWidget::setValue);
    w->setRange(-64, 64);
    w->setDefaultValue(0);
    ui->gridLayout_2->addWidget(w,row++,col);

    // Contrast
    modes.clear();
    modes.append(QPair<int, QString>(1,"Auto"));
    modes.append(QPair<int, QString>(0,"Auto"));
    w = new UVCControlWidget("Contrast", modes, "", this);
    connect(w, &UVCControlWidget::requestMode,
           capture->controls(), &UVCCaptureControls::getContrastMode);
    connect(w, &UVCControlWidget::modeChanged,
            capture->controls(), &UVCCaptureControls::setContrastMode);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getContrast);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setContrast);
    connect(capture->controls(), &UVCCaptureControls::contrastMode,
            w, &UVCControlWidget::setMode);
    connect(capture->controls(), &UVCCaptureControls::contrast,
            w, &UVCControlWidget::setValue);
    w->setRange(0, 100);
    w->setDefaultValue(50);
    ui->gridLayout_2->addWidget(w,row,col++);

    row = 0;

    // Hue
    modes.clear();
    modes.append(QPair<int, QString>(1,"Auto"));
    modes.append(QPair<int, QString>(0,"Auto"));
    w = new UVCControlWidget("Hue", modes, "", this);
    connect(w, &UVCControlWidget::requestMode,
           capture->controls(), &UVCCaptureControls::getHueMode);
    connect(w, &UVCControlWidget::modeChanged,
            capture->controls(), &UVCCaptureControls::setHueMode);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getHue);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setHue);
    connect(capture->controls(), &UVCCaptureControls::hueMode,
            w, &UVCControlWidget::setMode);
    connect(capture->controls(), &UVCCaptureControls::hue,
            w, &UVCControlWidget::setValue);
    w->setRange(-180, 180);
    w->setDefaultValue(0);
    ui->gridLayout_2->addWidget(w,row++,col);

    // Saturation
    modes.clear();
    w = new UVCControlWidget("Saturation", modes, "", this);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getSaturation);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setSaturation);
    connect(capture->controls(), &UVCCaptureControls::saturation,
            w, &UVCControlWidget::setValue);
    w->setRange(0, 100);
    w->setDefaultValue(64);
    ui->gridLayout_2->addWidget(w,row++,col);

    // Sharpness
    modes.clear();
    w = new UVCControlWidget("Sharpness", modes, "", this);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getSharpness);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setSharpness);
    connect(capture->controls(), &UVCCaptureControls::sharpness,
            w, &UVCControlWidget::setValue);
    w->setRange(0, 100);
    w->setDefaultValue(50);
    ui->gridLayout_2->addWidget(w,row++,col);

    // Gamma
    modes.clear();
    w = new UVCControlWidget("Gamma", modes, "", this);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getGamma);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setGamma);
    connect(capture->controls(), &UVCCaptureControls::gamma,
            w, &UVCControlWidget::setValue);
    w->setRange(100, 500);
    w->setDefaultValue(300);
    ui->gridLayout_2->addWidget(w,row++,col);

    // White Balance
    modes.clear();
    modes.append(QPair<int, QString>(1,"Auto"));
    modes.append(QPair<int, QString>(0,"Auto"));
    w = new UVCControlWidget("White Balance", modes, "", this);
    connect(w, &UVCControlWidget::requestMode,
           capture->controls(), &UVCCaptureControls::getWhiteBalanceMode);
    connect(w, &UVCControlWidget::modeChanged,
            capture->controls(), &UVCCaptureControls::setWhiteBalanceMode);
    connect(w, &UVCControlWidget::requestValue,
            capture->controls(), &UVCCaptureControls::getWhiteBalanceTemperature);
    connect(w, &UVCControlWidget::valueChanged,
            capture->controls(), &UVCCaptureControls::setWhiteBalanceTemperature);
    connect(capture->controls(), &UVCCaptureControls::whiteBalanceMode,
            w, &UVCControlWidget::setMode);
    connect(capture->controls(), &UVCCaptureControls::whiteBalanceTemperature,
            w, &UVCControlWidget::setValue);
    w->setRange(2800, 6500);
    w->setDefaultValue(4600);
    w->setDefaultMode(1);
    ui->gridLayout_2->addWidget(w,row++,col);
}

UVCCaptureSettings::~UVCCaptureSettings()
{
    delete ui;
}

void UVCCaptureSettings::startCapture()
{
    emit setCaptureActive(true);
}

void UVCCaptureSettings::stopCapture()
{
    emit setCaptureActive(false);
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
    qDebug()<<"device cb index changed"<<index<<"text"<<ui->comboBoxDevices->itemText(index);

    int vid = ui->comboBoxDevices->itemData(index, Qt::UserRole).toInt();
    int pid = ui->comboBoxDevices->itemData(index, Qt::UserRole + 1).toInt();
    QString serial = ui->comboBoxDevices->itemData(index, Qt::UserRole + 2).toString();
    if(vid != 0 && pid != 0) {
        emit openDevice(vid, pid, serial);
    }
}

void UVCCaptureSettings::on_comboBoxFormat_currentIndexChanged(int index)
{
    qDebug()<<"format cb index changed"<<index<<"text"<<ui->comboBoxFormat->itemText(index);
    QVariant v =  ui->comboBoxFormat->itemData(index,Qt::UserRole);
    UVCCapture::UVCCaptureProperties p = v.value<UVCCapture::UVCCaptureProperties>();
    emit capturePropertiesChanged(p);
}

void UVCCaptureSettings::on_pushButtonDefaults_clicked()
{
    const QObjectList object_list = ui->groupBox->children();
    foreach(QObject* object, object_list) {
        if(UVCControlWidget* ctrl = qobject_cast<UVCControlWidget*>(object)) {
            ctrl->resetToDefaultValues();
        }
    }
}
