#ifndef KITCHENTIMEREX_H
#define KITCHENTIMEREX_H

#include <QObject>
#include <QThread>

class KitchenTimerEx : public QObject
{
  Q_OBJECT

  //QML向けにプロパティを登録
  Q_PROPERTY(QString remainTimeString READ remainTimeString NOTIFY remainTimeStringChanged)
  Q_PROPERTY(int countTime READ countTime WRITE setCountTime NOTIFY countTimeChanged)
  Q_PROPERTY(bool fired READ fired NOTIFY firedChanged)
  Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
  explicit KitchenTimerEx(QObject *parent = nullptr);
  ~KitchenTimerEx();

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
  //カウント開始時のシグナル                    [1]
  void runTimer(int countTime);

public slots:
  void setRemainTimeString(QString remainTimeString);
  void setCountTime(int countTime);
  void setFired(bool fired);
  void setRunning(bool running);
  void start();
  void stop();
  void clear();
  //カウント終了時のスロット
  void finishedTimer(bool finished);

private:
  //プライベートクラス用の宣言                  [2]
  class Private;
  Private *d;

  Q_DISABLE_COPY(KitchenTimerEx)
};

#endif // KITCHENTIMEREX_H
