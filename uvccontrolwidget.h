#ifndef UVCCONTROLWIDGET_H
#define UVCCONTROLWIDGET_H

#include <QWidget>
#include "libuvc/libuvc.h"

namespace Ui {
class UVCControlWidget;
}

class QCheckBox;
class QComboBox;

class UVCControlWidget : public QWidget
{
    Q_OBJECT

public:
    UVCControlWidget(QString name, QVector<QPair<int, QString> >modes,
                     QString units = QString(), QWidget* parent = nullptr);
    ~UVCControlWidget();

signals:
    void requestMode();
    void modeChanged(int mode);
    void requestValue();
    void valueChanged(int value);

public slots:
    void setMode(int mode);
    void setValue(int value);
    void setRange(int min = 0, int max = 1024, int step = 1);
    void setDefaultValue(int value) {m_default_value = value;}
    void setDefaultMode(int mode) {m_default_mode = mode;}
    void resetToDefaultValues();


private slots:
    void onCheckBoxToggled(bool checked);

    void onComboBoxIndexChanged(bool index);

    void on_horizontalSlider_valueChanged(int value);

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::UVCControlWidget *ui;
    QCheckBox* m_check = nullptr;
    QComboBox* m_combo = nullptr;
    QVector<QPair<int, QString> > m_mode_data;
    int m_default_value = 0;
    int m_default_mode = 0;
    uvc_device_handle_t** m_handle = nullptr;
};

#endif // UVCCONTROLWIDGET_H
