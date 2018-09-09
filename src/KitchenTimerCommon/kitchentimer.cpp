#include "kitchentimer.h"

#include <QDateTime>
#include <QTimerEvent>

KitchenTimer::KitchenTimer(QObject *parent)
  : QObject(parent)
  , m_timerId(0)
  , m_remainTimeString(QStringLiteral("03:00"))
  , m_countTime(180000)
  , m_fired(false)
  , m_running(false)
{
}

QString KitchenTimer::remainTimeString() const
{
  return m_remainTimeString;
}

int KitchenTimer::countTime() const
{
  return m_countTime;
}

bool KitchenTimer::fired() const
{
  return m_fired;
}

bool KitchenTimer::running() const
{
  return m_running;
}

void KitchenTimer::setRemainTimeString(QString remainTimeString)
{
  if (m_remainTimeString == remainTimeString)
    return;

  m_remainTimeString = remainTimeString;
  emit remainTimeStringChanged(m_remainTimeString);
}

void KitchenTimer::setCountTime(int countTime)
{
  if (m_countTime == countTime)
    return;

  m_countTime = countTime;
  emit countTimeChanged(m_countTime);
}

void KitchenTimer::setFired(bool fired)
{
  if (m_fired == fired)
    return;

  m_fired = fired;
  emit firedChanged(m_fired);
}

void KitchenTimer::setRunning(bool running)
{
  if (m_running == running)
    return;

  m_running = running;
  emit runningChanged(m_running);
}

void KitchenTimer::start()
{
  if(m_timerId == 0){
    m_elapse.start();
    m_timerId = startTimer(100);
    setRunning(true);
  }
}

void KitchenTimer::stop()
{
  if(m_timerId != 0){
    killTimer(m_timerId);
    m_timerId = 0;
  }
}

void KitchenTimer::clear()
{
  stop();
  setFired(false);
  setRunning(false);
  updateRemainTime(countTime());
}

void KitchenTimer::timerEvent(QTimerEvent *e)
{
  if(e->timerId() == m_timerId){
    int remain = countTime() - static_cast<int>(m_elapse.elapsed());
    remain = qMin(remain, countTime());
    if(remain > 0){
      updateRemainTime(remain);
    }else{
      stop();
      setFired(true);
      updateRemainTime(0);
    }
  }
}

void KitchenTimer::updateRemainTime(int remainTime)
{
  setRemainTimeString(QTime::fromMSecsSinceStartOfDay(remainTime)
                      .toString("mm:ss"));
}
