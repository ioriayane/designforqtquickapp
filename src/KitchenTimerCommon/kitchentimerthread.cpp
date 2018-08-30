#include "kitchentimerthread.h"
#include <QDateTime>
#include <QDebug>
#include <QElapsedTimer>

class KitchenTimerThread::Private : public QObject
{
  Q_OBJECT
public:
  Private(KitchenTimerThread * parent);
  ~Private();

  QString makeRemainTimeString(int remainTime) const;

signals:
  void finished(bool success);
  void remainTimeStringChanged(QString remainTimeString);

public slots:
  void runTimer(int countTime);

private:
  KitchenTimerThread *q;
};


KitchenTimerThread::Private::Private(KitchenTimerThread *parent)
  : /*QObject()
  ,*/ q(parent)
{
  qDebug() << "Private()" << QThread::currentThreadId();
}

KitchenTimerThread::Private::~Private()
{
  qDebug() << "~Private()" << QThread::currentThreadId();
}

QString KitchenTimerThread::Private::makeRemainTimeString(int remainTime) const
{
  QDateTime d;
  d.setTime_t(static_cast<uint>(remainTime / 1000));
  return d.toString(QStringLiteral("mm:ss"));
}

void KitchenTimerThread::Private::runTimer(int countTime)
{
  qDebug() << "runTimer()" << QThread::currentThreadId();

  QElapsedTimer elapsetimer;
  elapsetimer.start();
  int remain;
  qint64 prev_update = 0;
  do {
    QThread::msleep(10);
    //経過時間を計算
    remain = countTime - static_cast<int>(elapsetimer.elapsed());
    remain = qMin(remain, countTime);
    if((elapsetimer.elapsed() - prev_update) > 100){
      //0.1sくらいごとに表示を更新
      emit remainTimeStringChanged(makeRemainTimeString(remain));
      prev_update = elapsetimer.elapsed();
    }
  }while(remain > 0 && q->running());

  if(q->running()){
    //カウントしきったらぴったり0にする
    emit remainTimeStringChanged(makeRemainTimeString(0));
  }else{
    //中断したときは初期値に戻す
    emit remainTimeStringChanged(makeRemainTimeString(q->countTime()));
  }
  //タイマーのカウント終了シグナル
  emit finished(q->running());
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

  //プライベートクラスをスレッドへ移動
  d->moveToThread(&dThread);

  //スレッドのイベントループ停止のシグナルでクラスを破棄
  connect(&dThread, &QThread::finished, d, &QObject::deleteLater);  //thisじゃなくてthreadなの注意
  //タイマー処理開始シグナルを接続
  connect(this, &KitchenTimerThread::runTimer, d, &Private::runTimer);
  //タイマー終了シグナルを接続
  connect(d, &Private::finished, this, &KitchenTimerThread::finishedTimer);
  //タイマーの時刻更新シグナルを接続
  connect(d, &Private::remainTimeStringChanged, this, &KitchenTimerThread::setRemainTimeString);

  //スレッドのイベントループ開始
  dThread.start();
}

KitchenTimerThread::~KitchenTimerThread()
{
  qDebug() << "~KitchenTimerThread()" << QThread::currentThreadId();
  //終了処理
  setRunning(false);
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

  qDebug() << "setRemainTimeString()" << remainTimeString << QThread::currentThreadId();

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
  setRunning(true);
  emit runTimer(countTime());
}

void KitchenTimerThread::stop()
{
}

void KitchenTimerThread::clear()
{
  setFired(false);
  if(running()){
    //動いてたらフラグを落として停止させる
    setRunning(false);
  }else{
    //すでに止まってたら表示だけリセット
    setRemainTimeString(d->makeRemainTimeString(countTime()));
  }
}

void KitchenTimerThread::finishedTimer(bool finished)
{
  qDebug() << "finishedTimer()" << finished << QThread::currentThreadId();
  setFired(finished);
  setRunning(finished);
}

//これを書くと通常は対象にならないcppファイルもmocが実行される
#include "kitchentimerthread.moc"
