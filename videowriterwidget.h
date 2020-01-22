#ifndef VIDEOWRITERWIDGET_H
#define VIDEOWRITERWIDGET_H

#include <QWidget>

namespace Ui {
class VideoWriterWidget;
}

class VideoWriterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWriterWidget(QWidget *parent = nullptr);
    ~VideoWriterWidget();

signals:
    void saveFrame(const QString &file_name);
    void startVideoRecording(const QString &file_name);
    void stopVideoRecording();

private slots:
    void on_pushButtonSaveFrame_clicked();

    void onSequenceTimerTimeout();
    void on_pushButtonSaveSequence_clicked();

    void on_pushButtonRecordVideo_clicked();

    void on_spinBoxSequenceInterval_editingFinished();

private:
    Ui::VideoWriterWidget *ui;
    QTimer* m_sequence_timer;
    QString m_base_file_name;
};

#endif // VIDEOWRITERWIDGET_H
