import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
// C++で公開登録したクラスをインポート
import tech.relog.plugin.kitchentimer 1.0
//システムトレイ用にインポート
import Qt.labs.platform 1.0
//設定保存用にインポート
import Qt.labs.settings 1.0

//ルートQMLタイプを変更して起動時にWindowを作らない   [1]
Item {
  ApplicationWindow {
    id: window
    visible: false
    width: 400
    height: 200
    title: qsTr("Kitchen Timer Station Mode")
    //背景色の変更
    color: timer.fired ? "#ee9999" : "white"

    //ウインドウ終了とアプリ終了を非連動にする        [2]
    onClosing: {
        close.accepted = false
        hide()
    }

    //残り時間の表示とスタート・クリアボタンの配置
    GridLayout {
      anchors.fill: parent
      anchors.margins: 10
      columns: 2
      Label {
        id: remainLabel
        Layout.fillWidth: true
        Layout.columnSpan: 2
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 64
        text: timer.remainTimeString
      }
      Button {
        id: startButton
        Layout.fillWidth: true
        text: "Start"
        enabled: !timer.running
        onClicked: timer.start()
      }
      Button {
        id: clearButton
        Layout.fillWidth: true
        text: "Clear"
        enabled: timer.running
        onClicked: timer.clear()
      }
      CheckBox {
        id: showCheckbox
        Layout.columnSpan: 2
        text: qsTr("Show at startup.")
        checked: settings.showAtStartup
      }
    }
    //残り時間をカウントするとタイマー機能
    KitchenTimer {
      id: timer
      countTime: 5000
      onFiredChanged: {
        if(fired){
          //お知らせメッセージを表示          [3]
          systemtrayicon.showMessage(qsTr("Kitchen Timer Station Mode")
                                     , qsTr("Time is up.")
                                     , SystemTrayIcon.Critical
                                     , 1000)
        }
      }
    }
    //ウインドウ表示                       [4]
    function forceShow(){
      show()
      raise()
      requestActivate()
    }
  }
  //システムトレイアイコンを表示                [5]
  SystemTrayIcon {
    id: systemtrayicon
    visible: true
    iconSource: "qrc:/icon.png"

    onActivated: {
      console.debug(reason)
      if(reason === SystemTrayIcon.DoubleClick){
        //ダブルクリックならウインドウ表示          [6]
        window.forceShow()
      }
    }
    //通知メッセージがクリックされたらウインドウ表示     [7]
    onMessageClicked: window.forceShow()
    //右クリック時のメニュー                     [8]
    menu: Menu {
      //ウインドウ表示
      MenuItem {
        text: qsTr("Show")
        onTriggered: window.forceShow()
      }
      //アプリ終了
      MenuItem {
        text: qsTr("Quit")
        onTriggered: Qt.quit()
      }
    }
  }
  //ウインドウの初期表示設定の保存               [9]
  Settings {
    id: settings
    property bool showAtStartup: true
  }
  Component.onCompleted: {
    if(settings.showAtStartup){
      //起動時に表示ならウインドウを起こす
      window.forceShow()
    }else{
      //起動時に非表示ならタイマースタート
      timer.start()
    }
  }
  Component.onDestruction: {
    //修了処理で設定を保存
    settings.showAtStartup = showCheckbox.checked
  }
}
