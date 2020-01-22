#include "uvccapturecontrols.h"
#include "libuvc/libuvc.h"

UVCCaptureControls::UVCCaptureControls(QObject* parent, uvc_device_handle_t *dev_handle) :
    QObject(parent), m_devh(dev_handle)
{}

void UVCCaptureControls::getExposureMode()
{
    uint8_t mode;
    uvc_get_ae_mode(m_devh, &mode, UVC_GET_CUR);
    emit exposureMode(mode);
}

void UVCCaptureControls::setExposureMode(uint8_t mode)
{
    uvc_set_ae_mode(m_devh, mode);
}

void UVCCaptureControls::getAbsExposure()
{

    uint32_t exposure_time;
    uvc_get_exposure_abs(m_devh, &exposure_time, UVC_GET_CUR);
    emit exposure(exposure_time);
}

void UVCCaptureControls::setAbsExposure(uint32_t exposure)
{
    uvc_set_exposure_abs(m_devh, exposure);
}

void UVCCaptureControls::getFocusMode()
{
    uint8_t state;
    uvc_get_focus_auto(m_devh, &state, UVC_GET_CUR);
    emit focusMode(state);
}

void UVCCaptureControls::setFocusMode(uint8_t mode)
{
    uvc_set_focus_auto(m_devh, mode);
}

void UVCCaptureControls::getAbsoluteFocus()
{
    uint16_t focus;
    uvc_get_focus_abs(m_devh, &focus, UVC_GET_CUR);
    emit absoluteFocus(focus);
}

void UVCCaptureControls::setAbsoluteFocus(uint16_t focus)
{
    uvc_set_focus_abs(m_devh, focus);
}

void UVCCaptureControls::getBackLightCompensation()
{

}

void UVCCaptureControls::setBackLightCompensation(uint16_t compensation)
{

}

void UVCCaptureControls::getBrightness()
{

}

void UVCCaptureControls::setBrightness(int16_t brightness)
{

}

void UVCCaptureControls::getContrastMode()
{

}

void UVCCaptureControls::setContrastMode(uint8_t mode)
{

}

void UVCCaptureControls::getContrast()
{

}

void UVCCaptureControls::setContrast(uint16_t contrast)
{

}

void UVCCaptureControls::getHueMode()
{

}

void UVCCaptureControls::setHueMode(uint8_t mode)
{

}

void UVCCaptureControls::getHue()
{

}

void UVCCaptureControls::setHue(int16_t hue)
{

}

void UVCCaptureControls::getSaturation()
{

}

void UVCCaptureControls::setSaturation(uint16_t saturation)
{

}

void UVCCaptureControls::getSharpness()
{

}

void UVCCaptureControls::setSharpness(uint16_t sharpness)
{

}

void UVCCaptureControls::getGamma()
{

}

void UVCCaptureControls::setGamma(uint16_t gamma)
{

}

void UVCCaptureControls::getWhiteBalanceMode()
{

}

void UVCCaptureControls::setWhiteBalanceMode(uint8_t mode)
{

}

void UVCCaptureControls::getWhiteBalanceTemperature()
{

}

void UVCCaptureControls::setWhiteBalanceTemperature(int16_t temperature)
{

}
