import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

ApplicationWindow {
  visible: true
  width: 400
  height: 200
  title: qsTr("Kitchen Timer leading by CPP1")

  //残り時間の表示とスタート・クリアボタンの配置
  GridLayout {
    anchors.fill: parent
    anchors.margins: 10
    columns: 2
    Text {
      objectName: "remainText"  //C++からQMLタイプを検索する目印 [1]
      Layout.fillWidth: true
      Layout.columnSpan: 2
      horizontalAlignment: Text.AlignHCenter
      font.pointSize: 64
    }
    Button {
      objectName: "startButton"  //C++からQMLタイプを検索する目印 [1]
      Layout.fillWidth: true
      text: "Start"
    }
    Button {
      objectName: "clearButton"  //C++からQMLタイプを検索する目印 [1]
      Layout.fillWidth: true
      text: "Clear"
    }
  }
}
