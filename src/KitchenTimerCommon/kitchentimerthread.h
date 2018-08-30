#ifndef KITCHENTIMERTHREAD_H
#define KITCHENTIMERTHREAD_H

#include <QObject>
#include <QThread>

class KitchenTimerThread : public QObject
{
  Q_OBJECT

  //QML向けにプロパティを登録
  Q_PROPERTY(QString remainTimeString READ remainTimeString NOTIFY remainTimeStringChanged)
  Q_PROPERTY(int countTime READ countTime WRITE setCountTime NOTIFY countTimeChanged)
  Q_PROPERTY(bool fired READ fired NOTIFY firedChanged)
  Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
  explicit KitchenTimerThread(QObject *parent = nullptr);
  ~KitchenTimerThread();

  QString remainTimeString() const;
  int countTime() const;
  bool fired() const;
  bool running() const;

signals:
  //プロパティ変化時のシグナル
  void remainTimeStringChanged(QString remainTimeString);
  void countTimeChanged(int countTime);
  void firedChanged(bool fired);
  void runningChanged(bool running);

  void runTimer(int countTime);

public slots:
  void setRemainTimeString(QString remainTimeString);
  void setCountTime(int countTime);
  void setFired(bool fired);
  void setRunning(bool running);
  void start();
  void stop();
  void clear();

  void finishedTimer(bool finished);

private:
  class Private;
  Private *d;
  QThread dThread;

  QString m_remainTimeString;
  int m_countTime;
  bool m_fired;
  bool m_running;
};

#endif // KITCHENTIMERTHREAD_H
