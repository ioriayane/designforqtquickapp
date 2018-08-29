#include "kitchentimerthread.h"
#include <QDebug>

class KitchenTimerThread::Private : public QObject
{
  Q_OBJECT
public:
  Private(KitchenTimerThread * parent);
  ~Private();

signals:
//  void finished(bool success);

public slots:
  void countTimer(int countTime);

private:
  KitchenTimerThread *q;
};


KitchenTimerThread::Private::Private(KitchenTimerThread *parent)
  : QObject(parent)
  , q(parent)
{
  qDebug() << "Private()" << QThread::currentThreadId();
}

KitchenTimerThread::Private::~Private()
{
  qDebug() << "~Private()" << QThread::currentThreadId();
}

void KitchenTimerThread::Private::countTimer(int countTime)
{
  qDebug() << "countTimer()" << QThread::currentThreadId();

//  emit finished(q->running());
}



KitchenTimerThread::KitchenTimerThread(QObject *parent)
  : QObject(parent)
  , d(new Private(this))
  , dThread(this)
  , m_remainTimeString(QStringLiteral("03:00"))
  , m_countTime(180000)
  , m_fired(false)
  , m_running(false)
{
  qDebug() << "KitchenTimerThread()" << QThread::currentThreadId();

  d->moveToThread(&dThread);

//  connect(d, &Private::finished, this, &KitchenTimerThread::finishedTimer);
  connect(this, &QObject::destroyed, this, &QObject::deleteLater);

  dThread.start();
}

KitchenTimerThread::~KitchenTimerThread()
{
  qDebug() << "~KitchenTimerThread()" << QThread::currentThreadId();

  dThread.exit();
  dThread.wait();
}

QString KitchenTimerThread::remainTimeString() const
{
  return m_remainTimeString;
}

int KitchenTimerThread::countTime() const
{
  return m_countTime;
}

bool KitchenTimerThread::fired() const
{
  return m_fired;
}

bool KitchenTimerThread::running() const
{
  return m_running;
}

void KitchenTimerThread::setRemainTimeString(QString remainTimeString)
{
  if (m_remainTimeString == remainTimeString)
    return;

  m_remainTimeString = remainTimeString;
  emit remainTimeStringChanged(m_remainTimeString);
}

void KitchenTimerThread::setCountTime(int countTime)
{
  if (m_countTime == countTime)
    return;

  m_countTime = countTime;
  emit countTimeChanged(m_countTime);
}

void KitchenTimerThread::setFired(bool fired)
{
  if (m_fired == fired)
    return;

  m_fired = fired;
  emit firedChanged(m_fired);
}

void KitchenTimerThread::setRunning(bool running)
{
  if (m_running == running)
    return;

  m_running = running;
  emit runningChanged(m_running);
}

void KitchenTimerThread::start()
{
//  if(m_timerId == 0){
//    m_elapse.start();
//    m_timerId = startTimer(100);
//    setRunning(true);
//  }
  setRunning(true);
  runTimer(countTime());
}

void KitchenTimerThread::stop()
{
//  if(m_timerId != 0){
//    killTimer(m_timerId);
//    m_timerId = 0;
//  }
}

void KitchenTimerThread::clear()
{
//  stop();
  setFired(false);
  setRunning(false);
  //  updateRemainTime(countTime());
}

void KitchenTimerThread::finishedTimer(bool success)
{
  qDebug() << "finishedTimer()" << QThread::currentThreadId();
  setFired(success);
  setRunning(false);
}
