#include "cameraview.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGestureEvent>
//#include <QOpenGLWidget>
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

    viewport()->grabGesture(Qt::PinchGesture);// for pinch-to-zoom

    // TK OpenGL causes a kernel panic when docking a dockwidget on
    // using: Intel Iris 5100 graphics
    // setViewport(new QOpenGLWidget());
    //setViewport(new QOpenGLWidget(this)); // QGraphicsView::QAbstractScrollArea takes ownership
}

void CameraView::scaleByFactor(qreal factor)
{
    qreal last_scale = m_scale_factor;
    qreal new_scale = last_scale * factor;
    if(new_scale < 0.1) new_scale = 0.1;

    setScaleFactor(new_scale);
}

void CameraView::setScaleFactor(const qreal scale_factor)
{
    QMatrix matrix;
    matrix.scale(scale_factor, scale_factor);
    setMatrix(matrix);
    m_scale_factor = scale_factor;
    qDebug()<<"Camera view scale factor set:"<<m_scale_factor;
}

void CameraView::showFrame(const QPixmap &frame)
{
    m_pixmap_frame->setPixmap(frame);
}

void CameraView::onFrameRectChanged(QRect rect)
{
    setSceneRect(rect);
}

bool CameraView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    else {
        return QGraphicsView::viewportEvent(event);
    }
}

bool CameraView::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}

void CameraView::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();

    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        currentStepScaleFactor = gesture->totalScaleFactor();
    }

    if (gesture->state() == Qt::GestureFinished) {
        setScaleFactor(m_scale_factor * currentStepScaleFactor);
        currentStepScaleFactor = 1;
        return;
    }
    // Compute the scale factor based on the current pinch level
    qreal sxy = m_scale_factor * currentStepScaleFactor;
    QMatrix matrix;
    matrix.scale(sxy, sxy);
    setMatrix(matrix);
}
