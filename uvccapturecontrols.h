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
    UVCCaptureControls(QObject* parent, uvc_device_handle_t *dev_handle);

signals:
    void exposureMode(uint8_t mode);
    void exposure(uint32_t exposure);

    void focusMode(uint8_t mode);
    void absoluteFocus(uint16_t focus);

    void backlightCompensation(uint16_t compensation);

    void brightness(int16_t);

    void contrastMode(uint8_t mode);
    void contrast(uint16_t contrast);

    void hueMode(uint8_t mode);
    void hue(int16_t hue);

    void saturation(uint16_t saturation);

    void sharpness(uint16_t sharpness);

    void gamma(uint16_t gamma);

    void whiteBalanceMode(uint8_t mode);
    void whiteBalanceTemperature(int16_t temperature);

public slots:
    //< exposure mode 8 = auto-exposure, 1 = manual exposure
    void getExposureMode();
    void setExposureMode(uint8_t mode);
    //< exposure time in 100us units (for 10ms, set to 100) range: 50 - 10000
    void getAbsExposure();
    void setAbsExposure(uint32_t exposure);

    //< focus mode 1 = auto-focus, 0 = manual focus
    void getFocusMode();
    void setFocusMode(uint8_t mode);
    //< focus distance in [mm] range: 0 - 1023
    void getAbsoluteFocus();
    void setAbsoluteFocus(uint16_t focus);

    //< 0 means backlight compensation is disabled. range: 0 - 1
    void getBackLightCompensation();
    void setBackLightCompensation(uint16_t compensation);

    //< range: 0 - 100
    void getBrightness();
    void setBrightness(int16_t brightness);

    //< unknown parameter range (guess 0 and 1?)
    void getContrastMode();
    void setContrastMode(uint8_t mode);
    //< range: -64 - 64
    void getContrast();
    void setContrast(uint16_t contrast);

    //< unknown parameter range (guess 0 and 1?)
    void getHueMode();
    void setHueMode(uint8_t mode);
    //< range: -180 - 180
    void getHue();
    void setHue(int16_t hue);

    //< range: 0 - 100
    void getSaturation();
    void setSaturation(uint16_t saturation);

    //< range: 0 - 100
    void getSharpness();
    void setSharpness(uint16_t sharpness);

    //< range: 100 - 500
    void getGamma();
    void setGamma(uint16_t gamma);

    //< unknown parameter range (guess 0 and 1?)
    void getWhiteBalanceMode();
    void setWhiteBalanceMode(uint8_t mode);
    //< range: 2800 - 6500
    void getWhiteBalanceTemperature();
    void setWhiteBalanceTemperature(int16_t temperature);
private:
    //< pointer to an open device. valid only when device is open
    uvc_device_handle_t *m_devh = nullptr;
};

#endif // UVCCAPTURECONTROLS_H
