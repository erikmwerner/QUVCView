#ifndef UVCCAPTURECONTROLS_H
#define UVCCAPTURECONTROLS_H

#include <QObject>
#include "libuvc/libuvc.h"

/*!
 * \brief The UVCCaptureControls class provides signals and slots
 * for thread-safe modification of capture controls.
 *
 * Ranges for controls are taken from values obtains from a
 * RXSX (RecordexUSA) UVC capture devie with an IMX179 sensor
 */
class UVCCaptureControls : public QObject
{
    Q_OBJECT

public:
    UVCCaptureControls(QObject* parent, uvc_device_handle_t **dev_handle);

signals:
    void exposureMode(int mode);
    void exposure(int exposure);

    void focusMode(int mode);
    void absoluteFocus(int focus);

    void backlightCompensation(int compensation);

    void brightness(int);

    void contrastMode(int mode);
    void contrast(int contrast);

    void hueMode(int mode);
    void hue(int hue);

    void saturation(int saturation);

    void sharpness(int sharpness);

    void gamma(int gamma);

    void whiteBalanceMode(int mode);
    void whiteBalanceTemperature(int temperature);

public slots:
    //< exposure mode 8 = auto-exposure, 1 = manual exposure
    void getExposureMode();
    void setExposureMode(int mode);
    //< exposure time in 100us units (for 10ms, set to 100) range: 50 - 10000
    void getAbsExposure();
    void setAbsExposure(int exposure);

    //< focus mode 1 = auto-focus, 0 = manual focus
    void getFocusMode();
    void setFocusMode(int mode);
    //< focus distance in [mm] range: 0 - 1023
    void getAbsoluteFocus();
    void setAbsoluteFocus(int focus);

    //< 0 means backlight compensation is disabled. range: 0 - 1
    void getBackLightCompensation();
    void setBackLightCompensation(int compensation);

    //< range: 0 - 100
    void getBrightness();
    void setBrightness(int brightness);

    //< unknown parameter range (guess 0 and 1?)
    void getContrastMode();
    void setContrastMode(int mode);
    //< range: -64 - 64
    void getContrast();
    void setContrast(int contrast);

    //< unknown parameter range (guess 0 and 1?)
    void getHueMode();
    void setHueMode(int mode);
    //< range: -180 - 180
    void getHue();
    void setHue(int hue);

    //< range: 0 - 100
    void getSaturation();
    void setSaturation(int saturation);

    //< range: 0 - 100
    void getSharpness();
    void setSharpness(int sharpness);

    //< range: 100 - 500
    void getGamma();
    void setGamma(int gamma);

    //< unknown parameter range (guess 0 and 1?)
    void getWhiteBalanceMode();
    void setWhiteBalanceMode(int mode);
    //< range: 2800 - 6500
    void getWhiteBalanceTemperature();
    void setWhiteBalanceTemperature(int temperature);
private:
    //< pointer to an open device handle. valid only when device is open
    uvc_device_handle_t **m_devh = nullptr;
};

#endif // UVCCAPTURECONTROLS_H
