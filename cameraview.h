#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QGraphicsView>
class QGraphicsPixmapItem;

class CameraView : public QGraphicsView
{
public:
    CameraView(QWidget *parent = Q_NULLPTR);

public slots:
    void showFrame(const QPixmap &frame);
    void onFrameRectChanged(QRect rect);

private:
    QGraphicsPixmapItem* m_pixmap_frame = nullptr;
    QGraphicsScene * m_scene = nullptr;
};

#endif // CAMERAVIEW_H
