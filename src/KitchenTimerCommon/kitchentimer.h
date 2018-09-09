﻿#ifndef KITCHENTIMER_H
#define KITCHENTIMER_H

#include <QObject>
#include <QElapsedTimer>

class KitchenTimer : public QObject
{
  Q_OBJECT

  //QML向けにプロパティを登録    [1]
  Q_PROPERTY(QString remainTimeString READ remainTimeString NOTIFY remainTimeStringChanged)
  Q_PROPERTY(int countTime READ countTime WRITE setCountTime NOTIFY countTimeChanged)
  Q_PROPERTY(bool fired READ fired NOTIFY firedChanged)
  Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
  explicit KitchenTimer(QObject *parent = nullptr);

  QString remainTimeString() const;
  int countTime() const;
  bool fired() const;
  bool running() const;

signals:
  //プロパティ変化時のシグナル   [2]
  void remainTimeStringChanged(QString remainTimeString);
  void countTimeChanged(int countTime);
  void firedChanged(bool fired);
  void runningChanged(bool running);

public slots:
  void setRemainTimeString(QString remainTimeString);
  void setCountTime(int countTime);
  void setFired(bool fired);
  void setRunning(bool running);
  void start();
  void stop();
  void clear();

protected:
  void timerEvent(QTimerEvent *e);

private:
  int m_timerId;

  QElapsedTimer m_elapse;
  QString m_remainTimeString;
  int m_countTime;
  bool m_fired;
  bool m_running;

  void updateRemainTime(int remainTime);

  Q_DISABLE_COPY(KitchenTimer)
};

#endif // KITCHENTIMER_H
