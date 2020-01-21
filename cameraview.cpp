#include "cameraview.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>

CameraView::CameraView(QWidget *parent) : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    this->setScene(m_scene);
    setSceneRect(QRect(0,0,640,480));
    QPixmap temp(640,480);
    temp.fill(Qt::green);
    m_pixmap_frame = new QGraphicsPixmapItem(temp);
    m_scene->addItem(m_pixmap_frame);
}

void CameraView::showFrame(const QPixmap &frame)
{
    m_pixmap_frame->setPixmap(frame);
}

void CameraView::onFrameRectChanged(QRect rect)
{
    setSceneRect(rect);
}
