#include "kitchentimer.h"

#include <QDateTime>

KitchenTimer::KitchenTimer(QObject *parent)
  : QObject(parent)
  , m_remainTimeString(QStringLiteral("03:00"))
  , m_countTime(180000)
  , m_fired(false)
  , m_running(false)
{
  connect(&m_timer, &QTimer::timeout, this, &KitchenTimer::triggered);
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
  m_elapse.start();
  m_timer.start(100);
  setRunning(true);
}

void KitchenTimer::clear()
{
  m_timer.stop();
  setFired(false);
  setRunning(false);
  updateRemainTime(countTime());
}

void KitchenTimer::triggered()
{
  int remain = countTime() - static_cast<int>(m_elapse.elapsed());
  remain = qMin(remain, countTime());
  if(remain > 0){
    updateRemainTime(remain);
  }else{
    updateRemainTime(0);
    m_timer.stop();
    setFired(true);
  }
}

void KitchenTimer::updateRemainTime(int remainTime)
{
  QDateTime d;
  d.setTime_t(static_cast<uint>(remainTime / 1000));
  setRemainTimeString(d.toString(QStringLiteral("mm:ss")));
}
