#include "fpstimer.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>

FPSTimer::FPSTimer(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    m_elapsed_timer = new QElapsedTimer();
    m_timer->setInterval(m_interval);
    connect(m_timer, &QTimer::timeout,
            this, &FPSTimer::onTimerTimeout);
}

void FPSTimer::onTimerTimeout()
{
    double seconds_elapsed = static_cast<double>(m_elapsed_timer->nsecsElapsed()) / 1000000000.0;
    m_elapsed_timer->start();
    double fps = static_cast<double>(m_frame_count) /seconds_elapsed;
    emit fpsUpdated(fps);
    m_frame_count = 0;
}

void FPSTimer::setInterval(int interval)
{
    m_interval = interval;
    m_timer->setInterval(interval);
}

void FPSTimer::setRunning(bool running)
{
    if(running) {
        m_timer->start(m_interval);
        m_elapsed_timer->start();
    }
    else {
        m_timer->stop();
        m_elapsed_timer->invalidate();
        emit fpsUpdated(0.0);
    }
}

bool FPSTimer::isRunning()
{
    return m_timer->isActive();
}
