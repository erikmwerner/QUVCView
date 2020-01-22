#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QGraphicsView>
class QGraphicsPixmapItem;
class QGestureEvent;
class QPinchGesture;

class CameraView : public QGraphicsView
{
public:
    CameraView(QWidget *parent = Q_NULLPTR);

    void scaleByFactor(qreal factor);
    qreal getScaleFactor(){return m_scale_factor;}
    void setScaleFactor(const qreal scale_factor);

public slots:
    void showFrame(const QPixmap &frame);
    void onFrameRectChanged(QRect rect);

signals:
    void scaleFactorChanged(qreal);

protected:
    bool viewportEvent(QEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);
    void pinchTriggered(QPinchGesture*gesture);

private:
    QGraphicsPixmapItem* m_pixmap_frame = nullptr;
    QGraphicsScene * m_scene = nullptr;

    qreal m_scale_factor = 1;
    qreal currentStepScaleFactor = 1;
};

#endif // CAMERAVIEW_H
