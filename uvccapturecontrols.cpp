#include "uvccapturecontrols.h"
#include "libuvc/libuvc.h"

#include <QDebug>

UVCCaptureControls::UVCCaptureControls(QObject* parent, uvc_device_handle_t **dev_handle) :
    QObject(parent), m_devh(dev_handle)
{}

void UVCCaptureControls::getExposureMode()
{
    if(*m_devh == nullptr) {return;}
    uint8_t mode;
    uvc_error_t res = uvc_get_ae_mode(*m_devh, &mode, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error gettings exposure mode"<<mode<<"Error:"<<res;
    }
    else {
        emit exposureMode(mode);
    }
}

void UVCCaptureControls::setExposureMode(int mode)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_ae_mode(*m_devh, static_cast<uint8_t>(mode));
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error settings exposure mode"<<mode<<"Error:"<<res;
    }
}

void UVCCaptureControls::getAbsExposure()
{
    if(*m_devh == nullptr) {return;}
    uint32_t value;
    uvc_error_t res = uvc_get_exposure_abs(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting abs exppsure"<<value<<"Error:"<<res;
    }
    else {
        emit exposure(value);
    }
}

void UVCCaptureControls::setAbsExposure(int exposure)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_exposure_abs(*m_devh, static_cast<uint32_t>(exposure));
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error setting abs exppsure"<<exposure<<"Error:"<<res;
    }
}

void UVCCaptureControls::getFocusMode()
{
    if(*m_devh == nullptr) {return;}
    uint8_t mode;
    uvc_error_t res = uvc_get_focus_auto(*m_devh, &mode, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting focus mode"<<mode<<"Error:"<<res;
    }
    else {
        emit focusMode(mode);
    }
}

void UVCCaptureControls::setFocusMode(int mode)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_focus_auto(*m_devh, static_cast<uint8_t>(mode));
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error setting focus mode"<<mode<<"Error:"<<res;
    }
}

void UVCCaptureControls::getAbsoluteFocus()
{
    if(*m_devh == nullptr) {return;}
    uint16_t value;
    uvc_error_t res = uvc_get_focus_abs(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting absolute focus"<<value<<"Error:"<<res;
    }
    else {
        emit absoluteFocus(value);
    }
}

void UVCCaptureControls::setAbsoluteFocus(int focus)
{
    uvc_error_t res = uvc_set_focus_abs(*m_devh, static_cast<uint16_t>(focus));
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error setting absolute focus"<<focus<<"Error:"<<res;
    }
}

void UVCCaptureControls::getBackLightCompensation()
{
    if(*m_devh == nullptr) {return;}
    uint16_t value;
    uvc_error_t res = uvc_get_backlight_compensation(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting backlight compensation"<<value<<"Error:"<<res;
    }
    else {
        emit backlightCompensation(value);
    }
}

void UVCCaptureControls::setBackLightCompensation(int compensation)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_backlight_compensation(*m_devh, static_cast<uint16_t>(compensation));
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error setting backlight compensation"<<compensation<<"Error:"<<res;
    }
}

void UVCCaptureControls::getBrightness()
{
    if(*m_devh == nullptr) {return;}
    int16_t value;
    uvc_error_t res = uvc_get_brightness(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting brightness"<<value<<"Error:"<<res;
    }
    else {
        emit brightness(value);
    }
}

void UVCCaptureControls::setBrightness(int brightness)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_brightness(*m_devh, static_cast<int16_t>(brightness));
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error setting brightness"<<brightness<<"Error:"<<res;
    }
}

void UVCCaptureControls::getContrastMode()
{
    if(*m_devh == nullptr) {return;}
    uint8_t mode;
    uvc_error_t res = uvc_get_contrast_auto(*m_devh, &mode, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting contrast mode"<<mode<<"Error:"<<res;
    }
    else {
        emit contrastMode(mode);
    }
}

void UVCCaptureControls::setContrastMode(int mode)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_contrast_auto(*m_devh, static_cast<uint8_t>(mode));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting contrast mode"<<mode<<"Error:"<<res;
    }
}

void UVCCaptureControls::getContrast()
{
    if(*m_devh == nullptr) {return;}
    uint16_t value;
    uvc_error_t res = uvc_get_contrast(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting contrast"<<value<<"Error:"<<res;
    }
    else {
        emit contrast(value);
    }
}

void UVCCaptureControls::setContrast(int contrast)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_contrast(*m_devh, static_cast<uint16_t>(contrast));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting contrast mode"<<contrast<<"Error:"<<res;
    }
}

void UVCCaptureControls::getHueMode()
{
    if(*m_devh == nullptr) {return;}
    uint8_t mode;
    uvc_error_t res = uvc_get_hue_auto(*m_devh, &mode, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting hue mode"<<mode<<"Error:"<<res;
    }
    else {
        emit hueMode(mode);
    }
}

void UVCCaptureControls::setHueMode(int mode)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_hue_auto(*m_devh, static_cast<uint8_t>(mode));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting hue mode"<<mode<<"Error:"<<res;
    }
}

void UVCCaptureControls::getHue()
{
    if(*m_devh == nullptr) {return;}
    int16_t value;
    uvc_error_t res = uvc_get_hue(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting contrast"<<value<<"Error:"<<res;
    }
    else {
        emit hue(value);
    }
}

void UVCCaptureControls::setHue(int hue)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_hue(*m_devh, static_cast<int16_t>(hue));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting hue mode"<<hue<<"Error:"<<res;
    }
}

void UVCCaptureControls::getSaturation()
{
    if(*m_devh == nullptr) {return;}
    uint16_t value;
    uvc_error_t res = uvc_get_saturation(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting saturation"<<value<<"Error:"<<res;
    }
    else {
        emit saturation(value);
    }
}

void UVCCaptureControls::setSaturation(int saturation)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_saturation(*m_devh, static_cast<uint16_t>(saturation));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting saturation mode"<<saturation<<"Error:"<<res;
    }
}

void UVCCaptureControls::getSharpness()
{
    if(*m_devh == nullptr) {return;}
    uint16_t value;
    uvc_error_t res = uvc_get_sharpness(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting sharpness"<<value<<"Error:"<<res;
    }
    else {
        emit sharpness(value);
    }
}

void UVCCaptureControls::setSharpness(int sharpness)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_sharpness(*m_devh, static_cast<uint16_t>(sharpness));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting sharpness"<<sharpness<<"Error:"<<res;
    }
}

void UVCCaptureControls::getGamma()
{
    if(*m_devh == nullptr) {return;}
    uint16_t value;
    uvc_error_t res = uvc_get_gamma(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting gamma"<<value<<"Error:"<<res;
    }
    else {
        emit gamma(value);
    }
}

void UVCCaptureControls::setGamma(int gamma)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_gamma(*m_devh, static_cast<uint16_t>(gamma));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting gama"<<gamma<<"Error:"<<res;
    }
}

void UVCCaptureControls::getWhiteBalanceMode()
{
    if(*m_devh == nullptr) {return;}
    uint8_t mode;
    uvc_error_t res = uvc_get_white_balance_temperature_auto(*m_devh, &mode, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting white balance mode"<<mode<<"Error:"<<res;
    }
    else {
        emit whiteBalanceMode(mode);
    }
}

void UVCCaptureControls::setWhiteBalanceMode(int mode)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_white_balance_temperature(*m_devh, static_cast<uint8_t>(mode));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting white balance mode"<<mode<<"Error:"<<res;
    }
}

void UVCCaptureControls::getWhiteBalanceTemperature()
{
    if(*m_devh == nullptr) {return;}
    uint16_t value;
    uvc_error_t res = uvc_get_white_balance_temperature(*m_devh, &value, UVC_GET_CUR);
    if(res < UVC_SUCCESS) {
        qDebug()<<"Error getting white balance temperature"<<value<<"Error:"<<res;
    }
    else {
        emit whiteBalanceTemperature(value);
    }
}

void UVCCaptureControls::setWhiteBalanceTemperature(int temperature)
{
    if(*m_devh == nullptr) {return;}
    uvc_error_t res = uvc_set_white_balance_temperature(*m_devh, static_cast<uint16_t>(temperature));
    if(res < UVC_SUCCESS) {
         qDebug()<<"Error setting white balance temperature"<<temperature<<"Error:"<<res;
    }
}
