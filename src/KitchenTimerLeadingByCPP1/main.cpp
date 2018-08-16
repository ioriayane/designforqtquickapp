#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QMetaObject>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QDebug>
#include "kitchentimer.h"

int main(int argc, char *argv[])
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;

  KitchenTimer timer;
  //QML主導のときプロパティバインディングしていたところ相当の実装
  //ルートオブジェクトの取得                                            [1]
  QQuickWindow *root =
      qobject_cast<QQuickWindow *>(engine.rootObjects().first());
  //時間表示とボタンのQMLタイプのポインタを取得                               [2]
  QQuickItem *remain = root->findChild<QQuickItem *>("remainLabel");
  QQuickItem *start = root->findChild<QQuickItem *>("startButton");
  QQuickItem *clear = root->findChild<QQuickItem *>("clearButton");
  //タイマー機能の値の変化をQML側にセット                                   [3]
  QObject::connect(&timer, &KitchenTimer::remainTimeStringChanged
                   , [remain](const QString &remainTimeString){
    remain->setProperty("text", remainTimeString);
  });
  QObject::connect(&timer, &KitchenTimer::runningChanged
                   , [start](bool running){
    start->setEnabled(!running);
  });
  QObject::connect(&timer, &KitchenTimer::runningChanged
                   , [clear](bool running){
    clear->setEnabled(running);
  });
  QObject::connect(&timer, &KitchenTimer::firedChanged
                   , [root](bool fired){
    if(fired){
      root->setColor("#ee9999");
    }else{
      root->setColor("#ffffff");
    }
  });
  //QML側の操作をタイマー機能に伝える                                      [4]
  QObject::connect(start, SIGNAL(clicked()), &timer, SLOT(start()));
  QObject::connect(clear, SIGNAL(clicked()), &timer, SLOT(clear()));

  //初期状態                                                        [5]
  QVariant returnedValue;
  QMetaObject::invokeMethod(root, "initialize"
                            , Q_RETURN_ARG(QVariant, returnedValue)
                            , Q_ARG(QVariant, timer.remainTimeString())
                            , Q_ARG(QVariant, timer.running())
                            );
  qDebug() << returnedValue.toBool();
  //以下でも良い
//  remain->setProperty("text", timer.remainTimeString());
//  clear->setEnabled(timer.running());

  return app.exec();
}
