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
    void saveFrame();

private slots:
    void on_pushButtonSaveFrame_clicked();

private:
    Ui::VideoWriterWidget *ui;
};

#endif // VIDEOWRITERWIDGET_H
