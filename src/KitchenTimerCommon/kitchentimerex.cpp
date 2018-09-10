#include "kitchentimerex.h"
#include <QDateTime>
#include <QElapsedTimer>
#include <QDebug>

//--------------------------------------------------------------
// サブスレッドで残り時間をカウントするクラス
//--------------------------------------------------------------
class KitchenTimerCounter : public QObject
{
  Q_OBJECT
public:
  KitchenTimerCounter(KitchenTimerEx *parent);
  ~KitchenTimerCounter();

  QString makeRemainTimeString(int remainTime) const;

signals:
  //カウント終了と残り時間の更新シグナル            [1]
  void finished(bool success);
  void remainTimeStringChanged(QString remainTimeString);

public slots:
  void runTimer(int countTime);

private:
  KitchenTimerEx *q;     //親クラスのポインタ     [2]

  Q_DISABLE_COPY(KitchenTimerCounter)
};

KitchenTimerCounter::KitchenTimerCounter(KitchenTimerEx *parent)
  : q(parent)           //メンバの初期化    [3]
{
  qDebug() << "KitchenTimerCounter()" << QThread::currentThreadId();
}

KitchenTimerCounter::~KitchenTimerCounter()
{
  qDebug() << "~KitchenTimerCounter()" << QThread::currentThreadId();
}

QString KitchenTimerCounter::makeRemainTimeString(int remainTime) const
{
  return QTime::fromMSecsSinceStartOfDay(remainTime).toString("mm:ss");
}

//カウント処理     [4]
void KitchenTimerCounter::runTimer(int countTime)
{
  qDebug() << "runTimer()" << QThread::currentThreadId();

  QElapsedTimer elapsetimer;
  elapsetimer.start();
  int remain;
  qint64 curr_update = 0;
  qint64 prev_update = 0;
  do {
    QThread::msleep(10);
    //経過時間を計算
    remain = countTime - static_cast<int>(elapsetimer.elapsed());
    remain = qMin(remain, countTime);
    curr_update = elapsetimer.elapsed();
    if((curr_update - prev_update) > 100){
      //0.1sくらいごとに表示を更新
      emit remainTimeStringChanged(makeRemainTimeString(remain));
      prev_update = curr_update;
    }
  }while(remain > 0 && q->running());

  //タイマーのカウント終了シグナル
  emit finished(q->running());
}

//--------------------------------------------------------------
// キッチンタイマークラスの実体（プライベートクラス）
//--------------------------------------------------------------
class KitchenTimerEx::Private
{
public:
  Private(KitchenTimerEx * parent);
  ~Private();

  QString remainTimeString() const;
  int countTime() const;
  bool fired() const;
  bool running() const;

  void setRemainTimeString(QString remainTimeString);
  void setCountTime(int countTime);
  void setFired(bool fired);
  void setRunning(bool running);
  void start();
  void stop();
  void clear();
  void finishedTimer(bool finished);

private:
  KitchenTimerEx *q;            //親クラスのポインタ     [2]
  KitchenTimerCounter *counter; //残り時間のカウントクラス
  QThread *thread;              //スレッドクラス

  QString m_remainTimeString;
  int m_countTime;
  bool m_fired;
  bool m_running;
};


KitchenTimerEx::Private::Private(KitchenTimerEx *parent)
  : q(parent)
  , counter(new KitchenTimerCounter(parent))
  , thread(new QThread(parent))
  , m_remainTimeString(QStringLiteral("03:00"))
  , m_countTime(180000)
  , m_fired(false)
  , m_running(false)
{
  qDebug() << "Private()" << QThread::currentThreadId();

  //プライベートクラスをスレッドへ移動               [5]
  counter->moveToThread(thread);

  //各種シグナルの接続                       [6]
  //スレッドのイベントループ停止のシグナルでクラスの破棄をスケジュール
  connect(thread, &QThread::finished
          , counter, &QObject::deleteLater);
  //タイマー処理開始シグナルを接続
  connect(q, &KitchenTimerEx::runTimer
          , counter, &KitchenTimerCounter::runTimer);
  //タイマー終了シグナルを接続
  connect(counter, &KitchenTimerCounter::finished
          , q, &KitchenTimerEx::finishedTimer);
  //タイマーの時刻更新シグナルを接続
  connect(counter, &KitchenTimerCounter::remainTimeStringChanged
          , q, &KitchenTimerEx::setRemainTimeString);

  //スレッドのイベントループ開始                  [7]
  thread->start();
}

KitchenTimerEx::Private::~Private()
{
  qDebug() << "~Private()" << QThread::currentThreadId();
  //終了処理                             [8]
  setRunning(false);
  thread->exit();
  thread->wait();
}


QString KitchenTimerEx::Private::remainTimeString() const
{
  return m_remainTimeString;
}

int KitchenTimerEx::Private::countTime() const
{
  return m_countTime;
}

bool KitchenTimerEx::Private::fired() const
{
  return m_fired;
}

bool KitchenTimerEx::Private::running() const
{
  return m_running;
}

void KitchenTimerEx::Private::setRemainTimeString(QString remainTimeString)
{
  if (m_remainTimeString == remainTimeString)
    return;

  qDebug() << "setRemainTimeString()" << remainTimeString
           << QThread::currentThreadId();

  m_remainTimeString = remainTimeString;
  emit q->remainTimeStringChanged(m_remainTimeString);
}

void KitchenTimerEx::Private::setCountTime(int countTime)
{
  if (m_countTime == countTime || running())
    return;

  m_countTime = countTime;
  emit q->countTimeChanged(m_countTime);

  //設定時間が変更されたら表示にも反映
  setRemainTimeString(counter->makeRemainTimeString(m_countTime));
}

void KitchenTimerEx::Private::setFired(bool fired)
{
  if (m_fired == fired)
    return;

  m_fired = fired;
  emit q->firedChanged(m_fired);
}

void KitchenTimerEx::Private::setRunning(bool running)
{
  if (m_running == running)
    return;

  m_running = running;
  emit q->runningChanged(m_running);
}

void KitchenTimerEx::Private::start()
{
  setRunning(true);
  emit q->runTimer(countTime());
}

void KitchenTimerEx::Private::stop()
{
}

void KitchenTimerEx::Private::clear()
{
  if(fired()){
    //カウントが終わってたら表示をリセット
    setRemainTimeString(counter->makeRemainTimeString(countTime()));
  }
  //カウント状況をクリア（カウント中ならフラグを落として停止も）
  setFired(false);
  setRunning(false);
}

void KitchenTimerEx::Private::finishedTimer(bool finished)
{
  qDebug() << "finishedTimer()" << finished << QThread::currentThreadId();
  if(finished){
    //カウントしきってたらぴったり0にする
    setRemainTimeString(counter->makeRemainTimeString(0));
  }else{
    //中断したときは初期値に戻す
    setRemainTimeString(counter->makeRemainTimeString(countTime()));
  }
  //最後までカウントしているかでtrue/falseを決める
  setFired(finished);
  setRunning(finished);
}

//--------------------------------------------------------------
// キッチンタイマークラス
//--------------------------------------------------------------
KitchenTimerEx::KitchenTimerEx(QObject *parent)
  : QObject(parent)
  , d(new Private(this))    //プライベートクラスのインスタンス化
{
  qDebug() << "KitchenTimerEx()" << QThread::currentThreadId();
}

KitchenTimerEx::~KitchenTimerEx()
{
  qDebug() << "~KitchenTimerEx()" << QThread::currentThreadId();
  //プライベートクラスの破棄
  delete d;
}

QString KitchenTimerEx::remainTimeString() const
{
  return d->remainTimeString();
}

int KitchenTimerEx::countTime() const
{
  return d->countTime();
}

bool KitchenTimerEx::fired() const
{
  return d->fired();
}

bool KitchenTimerEx::running() const
{
  return d->running();
}

void KitchenTimerEx::setRemainTimeString(QString remainTimeString)
{
  d->setRemainTimeString(remainTimeString);
}

void KitchenTimerEx::setCountTime(int countTime)
{
  d->setCountTime(countTime);
}

void KitchenTimerEx::setFired(bool fired)
{
  d->setFired(fired);
}

void KitchenTimerEx::setRunning(bool running)
{
  d->setRunning(running);
}

void KitchenTimerEx::start()
{
  d->start();
}

void KitchenTimerEx::stop()
{
  d->stop();
}

void KitchenTimerEx::clear()
{
  d->clear();
}

void KitchenTimerEx::finishedTimer(bool finished){
  d->finishedTimer(finished);
}

//mocの対象にする                     [9]
#include "kitchentimerex.moc"

