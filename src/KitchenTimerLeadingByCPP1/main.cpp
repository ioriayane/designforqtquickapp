#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickItem>
#include "kitchentimer.h"

#include <QDebug>

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
  QObject *root = engine.rootObjects().first();
  QObject *remain = root->findChild<QObject *>("remainText");
  QObject *start = root->findChild<QObject *>("startButton");
  QObject *clear = root->findChild<QObject *>("clearButton");
  //標準QMLタイプのセッターがスロットになっていないのでラムダ式で受けてセットする
  QObject::connect(&timer, &KitchenTimer::remainTimeStringChanged, [remain](const QString &remainTimeString){
    remain->setProperty("text", remainTimeString);
  });
  QObject::connect(&timer, &KitchenTimer::runningChanged, [start](bool running){
    start->setProperty("enabled", !running);
  });
  QObject::connect(&timer, &KitchenTimer::runningChanged, [clear](bool running){
    clear->setProperty("enabled", running);
  });
  //標準QMLタイプにはクラスのヘッダーをincludeできない場合があるので文字列ベースでコネクトする
  QObject::connect(start, SIGNAL(clicked()), &timer, SLOT(start()));
  QObject::connect(clear, SIGNAL(clicked()), &timer, SLOT(clear()));
  //背景色の変更
  QObject::connect(&timer, &KitchenTimer::firedChanged, [root](bool fired){
    if(fired){
      root->setProperty("color", QColor("#ee9999"));
    }else{
      root->setProperty("color", QColor("#ffffff"));
    }
  });

  //初期状態
  remain->setProperty("text", timer.remainTimeString());
  clear->setProperty("enabled", false);

  return app.exec();
}
