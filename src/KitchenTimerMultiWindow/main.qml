import QtQuick 2.11

Item {
  //QMLタイプを繰り返し展開           [1]
  Repeater {
    id: repeater
    //繰り返し展開する内容の管理モデル  [2]
    model: ListModel {}
    //QMLタイプの動的読み込み         [3]
    Loader {
      id: loader
      //読み込み対象                [4]
      sourceComponent: kitchenTimerWindow
      onLoaded: {
        //カウント時間の設定とウインドウ表示 [5]
        item.countTime = model.countTime
        item.show()
      }
    }
  }
  //動的読み込みの準備としての定義      [6]
  Component {
    id: kitchenTimerWindow
    KitchenTimerWindow {
      onStarted: console.debug("started:" + countTime)
    }
  }

  Component.onCompleted: {
    //カウント時間違いで追加             [7]
    repeater.model.append({"countTime": 5000})
    repeater.model.append({"countTime": 10000})
  }
}
