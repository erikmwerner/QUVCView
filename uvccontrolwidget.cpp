#include "uvccontrolwidget.h"
#include "ui_uvccontrolwidget.h"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QDebug>

UVCControlWidget::UVCControlWidget(QString name, QVector<QPair<int, QString> >modes,
                                   QString units, QWidget* parent) :
    QWidget(parent), ui(new Ui::UVCControlWidget), m_mode_data(modes)
{
    ui->setupUi(this);
    setObjectName(name);
    ui->label->setText(name);
    if(!units.isEmpty()) {
        ui->spinBox->setSuffix(units);
    }
    if(modes.length() == 2) {
        // two possible modes, use a checkbox
        m_check = new QCheckBox(this);
        // use the first mode name as the checkbox name
        m_check->setText(modes.first().second);
        connect(m_check, &QCheckBox::toggled,
                this,&UVCControlWidget::onCheckBoxToggled);
        ui->gridLayout->addWidget(m_check,0,1);
    }
    else if(modes.length() > 2) {
        // for 3+ modes, use a combo box
        m_combo = new QComboBox(this);
        for(int i = 0; i <modes.length(); ++i){
            QPair<int, QString> mode = modes.at(i);
            m_combo->addItem(mode.second, mode.first);
        }
        ui->gridLayout->addWidget(m_combo,0,1);
    }
    else {
        // mode list is empty
    }
    QFont font;
    font.setPointSize(10);
    setFont(font);
}

UVCControlWidget::~UVCControlWidget()
{
    delete ui;
}

void UVCControlWidget::setMode(int mode)
{
    if(m_check != nullptr) {
        m_check->setChecked(mode == m_mode_data.first().first ? true : false);
    }
    else if(m_combo != nullptr)
    {
        for(int i = 0; i <m_mode_data.length(); ++i){
            QPair<int, QString> m = m_mode_data.at(i);
            if(m.first == mode) {
                m_combo->setCurrentIndex(i);
            }
        }
    }
}

void UVCControlWidget::setValue(int value)
{
    const QSignalBlocker blocker1(ui->spinBox);
    const QSignalBlocker blocker2(ui->horizontalSlider);
    ui->spinBox->setValue(value);
    ui->horizontalSlider->setValue(value);
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

void UVCControlWidget::resetToDefaultValues()
{
    if(m_mode_data.length() == 2) {
        QPair<int, QString> m = m_mode_data.at(0);
        m_check->setChecked(m.first == m_default_mode);
    }
    else if(m_mode_data.length() > 2) {
        for(int i = 0; i <m_mode_data.length(); ++i){
            QPair<int, QString> m = m_mode_data.at(i);
            if(m.first == m_default_mode) {
                m_combo->setCurrentIndex(i);
            }
        }
    }
    ui->horizontalSlider->setValue(m_default_value);
}


void UVCControlWidget::onCheckBoxToggled(bool checked)
{
    if(checked) {
        // disable slider and spinbox
        ui->horizontalSlider->setEnabled(false);
        ui->spinBox->setEnabled(false);
        emit modeChanged(m_mode_data.at(0).first);
    }
    else {
        // enable slider and spinbox
        ui->horizontalSlider->setEnabled(true);
        ui->spinBox->setEnabled(true);
        emit modeChanged(m_mode_data.at(1).first);
    }
}

void UVCControlWidget::onComboBoxIndexChanged(bool index)
{
    emit modeChanged(m_mode_data.at(index).first);
}

void UVCControlWidget::on_horizontalSlider_valueChanged(int value)
{
    const QSignalBlocker blocker(ui->spinBox);
    ui->spinBox->setValue(value);
    emit valueChanged(value);
}

void UVCControlWidget::on_spinBox_valueChanged(int arg1)
{
    const QSignalBlocker bloccker(ui->horizontalSlider);
    ui->horizontalSlider->setValue(arg1);
    emit valueChanged(arg1);
}
