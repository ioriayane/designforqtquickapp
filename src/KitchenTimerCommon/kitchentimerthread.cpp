﻿#include "kitchentimerthread.h"
#include <QDateTime>
#include <QElapsedTimer>
#include <QDebug>

class KitchenTimerThread::Private : public QObject
{
  Q_OBJECT
public:
  Private(KitchenTimerThread * parent);
  ~Private();

  QString makeRemainTimeString(int remainTime) const;

signals:
  //カウント終了と残り時間の更新シグナル            [1]
  void finished(bool success);
  void remainTimeStringChanged(QString remainTimeString);

public slots:
  void runTimer(int countTime);

private:
  KitchenTimerThread *q;  // 親クラスのポインタ [2]
};


KitchenTimerThread::Private::Private(KitchenTimerThread *parent)
  : q(parent)           //メンバの初期化    [3]
{
  qDebug() << "Private()" << QThread::currentThreadId();
}

KitchenTimerThread::Private::~Private()
{
  qDebug() << "~Private()" << QThread::currentThreadId();
}

QString KitchenTimerThread::Private::makeRemainTimeString
(int remainTime) const
{
  QDateTime d;
  d.setTime_t(static_cast<uint>(remainTime / 1000));
  return d.toString(QStringLiteral("mm:ss"));
}

//カウント処理     [4]
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
  , d(new Private(this))    //プライベートクラスのインスタンス化
  , dThread(this)
  , m_remainTimeString(QStringLiteral("03:00"))
  , m_countTime(180000)
  , m_fired(false)
  , m_running(false)
{
  qDebug() << "KitchenTimerThread()" << QThread::currentThreadId();

  //プライベートクラスをスレッドへ移動               [5]
  d->moveToThread(&dThread);

  //各種シグナルの接続                       [6]
  //スレッドのイベントループ停止のシグナルでクラスを破棄
  connect(&dThread, &QThread::finished, d, &QObject::deleteLater);
  //タイマー処理開始シグナルを接続
  connect(this, &KitchenTimerThread::runTimer, d, &Private::runTimer);
  //タイマー終了シグナルを接続
  connect(d, &Private::finished, this, &KitchenTimerThread::finishedTimer);
  //タイマーの時刻更新シグナルを接続
  connect(d, &Private::remainTimeStringChanged
          , this, &KitchenTimerThread::setRemainTimeString);

  //スレッドのイベントループ開始                  [7]
  dThread.start();
}

KitchenTimerThread::~KitchenTimerThread()
{
  qDebug() << "~KitchenTimerThread()" << QThread::currentThreadId();
  //終了処理                              [8]
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

  qDebug() << "setRemainTimeString()" << remainTimeString
           << QThread::currentThreadId();

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
  if(fired()){
    //カウントが終わってたら表示をリセット
    setRemainTimeString(d->makeRemainTimeString(countTime()));
  }
  //カウント状況をクリア（カウント中ならフラグを落として停止も）
  setFired(false);
  setRunning(false);
}

void KitchenTimerThread::finishedTimer(bool finished)
{
  qDebug() << "finishedTimer()" << finished << QThread::currentThreadId();
  //最後までカウントしているかでtrue/falseを決める
  setFired(finished);
  setRunning(finished);
}

//mocの対象にする                     [9]
#include "kitchentimerthread.moc"
