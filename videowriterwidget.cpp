#include "videowriterwidget.h"
#include "ui_videowriterwidget.h"

VideoWriterWidget::VideoWriterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoWriterWidget)
{
    ui->setupUi(this);
}

VideoWriterWidget::~VideoWriterWidget()
{
    delete ui;
}

void VideoWriterWidget::on_pushButtonSaveFrame_clicked()
{
    emit saveFrame();
}
