#ifndef UVCCAPTURESETTINGS_H
#define UVCCAPTURESETTINGS_H

#include <QWidget>
#include "libuvc/libuvc.h"
#include "uvccapture.h"

namespace Ui {
class UVCCaptureSettings;
}

class UVCCaptureSettings : public QWidget
{
    Q_OBJECT

public:
    explicit UVCCaptureSettings(UVCCapture *parent = nullptr);
    ~UVCCaptureSettings();

public slots:
    void onSupportedFormatsFound(QVector<UVCCapture::UVCCaptureProperties>);
signals:
    void openDevice(int, int, QString);
    void capturePropertiesChanged(UVCCapture::UVCCaptureProperties p);

protected slots:
    void onDevicesFound(QVector<UVCCapture::UVCCaptureDescriptor> devices);

private slots:


    void on_comboBoxDevices_currentIndexChanged(int index);

    void on_comboBoxFormat_currentIndexChanged(int index);

private:
    Ui::UVCCaptureSettings *ui;
    UVCCapture* m_parent;
};

#endif // UVCCAPTURESETTINGS_H
