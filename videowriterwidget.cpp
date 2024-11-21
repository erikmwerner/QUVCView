#include "videowriterwidget.h"
#include "ui_videowriterwidget.h"
#include <QTimer>
#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardPaths>

VideoWriterWidget::VideoWriterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoWriterWidget)
{
    ui->setupUi(this);
    m_sequence_timer = new QTimer(this);
    connect(m_sequence_timer, &QTimer::timeout,
            this, &VideoWriterWidget::onSequenceTimerTimeout);
}

VideoWriterWidget::~VideoWriterWidget()
{
    delete ui;
}

void VideoWriterWidget::on_pushButtonSaveFrame_clicked()
{
    QSettings settings;
    QString dir;
    if(settings.contains("lastImageSaveLocation")) {
        dir = settings.value("lastImageSaveLocation").toString();
    }
    else {
        dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Frame"),
                                                     dir, tr("Images (*.png *.xpm *.jpg)"));
    if(!file_name.isEmpty()) {
        emit saveFrame(file_name);
        settings.setValue("lastImageSaveLocation", file_name);
    }

}

void VideoWriterWidget::onSequenceTimerTimeout()
{
    QFileInfo info(m_base_file_name);
    QString file_path = info.absolutePath();
    QString file_name_base = info.baseName();
    QString file_name_suffix = info.suffix();
    QString suffix_text = ui->lineEditSuffix->text();
    int value = ui->spinBoxSequenceStart->value();
    QString index = QString::number(value);

    QString file_name = file_path + '/' + file_name_base
            + suffix_text + index + "." + file_name_suffix;

    emit saveFrame(file_name);

    ui->spinBoxSequenceStart->setValue(++value);
}

void VideoWriterWidget::on_pushButtonSaveSequence_clicked()
{
    if(m_sequence_timer->isActive()) {
        m_sequence_timer->stop();
        ui->pushButtonSaveSequence->setText("Save Sequence...");
        ui->pushButtonSaveSequence->setDefault(false);
    }
    else {
        QSettings settings;
        QString dir;
        if(settings.contains("lastImageSequenceLocation")) {
            dir = settings.value("lastImageSequenceLocation").toString();
        }
        else {
            dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        }
        QString file_name = QFileDialog::getSaveFileName(this, tr("Save Sequence"),
                                                         dir, tr("Images (*.png *.xpm *.jpg)"));
        if(!file_name.isEmpty()) {
            m_base_file_name = file_name;
            onSequenceTimerTimeout();
            m_sequence_timer->start(ui->spinBoxSequenceInterval->value());
            ui->pushButtonSaveSequence->setText("Stop Sequence");
            ui->pushButtonSaveSequence->setDefault(true);
            settings.setValue("lastImageSequenceLocation", file_name);
        }
    }
}

void VideoWriterWidget::on_pushButtonRecordVideo_clicked()
{
    if( ui->pushButtonRecordVideo->text() == "Record Video..."){
        QSettings settings;
        QString dir;
        if(settings.contains("lastVideoSaveLocation")) {
            dir = settings.value("lastVideoSaveLocation").toString();
        }
        else {
            dir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        }
        QString file_name = QFileDialog::getSaveFileName(this, tr("Save Video"),
                                                         dir, tr("Videos (*.mp4)"));
        if(!file_name.isEmpty()) {
            settings.setValue("lastVideoSaveLocation", file_name);
            emit startVideoRecording(file_name);
            ui->pushButtonRecordVideo->setText("Stop Recording");
        }
    }
    else {
        emit stopVideoRecording();
        ui->pushButtonRecordVideo->setText("Record Video...");
    }
}

void VideoWriterWidget::on_spinBoxSequenceInterval_editingFinished()
{
    m_sequence_timer->setInterval(ui->spinBoxSequenceInterval->value());
}
