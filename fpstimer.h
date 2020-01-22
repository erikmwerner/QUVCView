#ifndef FPSTIMER_H
#define FPSTIMER_H

#include <QObject>
class QTimer;
class QElapsedTimer;

/*!
 * \brief The FPSTimer class tracks the number
 * of frames that occur over a time interval to
 * calculate the average frames per second over
 * that interval
 */
class FPSTimer : public QObject
{
    Q_OBJECT
public:
    explicit FPSTimer(QObject *parent = nullptr);

public slots:
    void incrementFrame() { m_frame_count++; }
    void setInterval(int interval);
    void setRunning(bool running);
    bool isRunning();

signals:
    void fpsUpdated(double);

private slots:
    void onTimerTimeout();

private:
     QTimer* m_timer = nullptr;
     QElapsedTimer* m_elapsed_timer = nullptr;
     int m_interval = 1000;
     int m_frame_count = 0;
};

#endif // FPSTIMER_H
