import QtQuick 2.11

Timer {
  property string remainTimeString: "03:00"
  property var countTime: 180000
  property var startTime: 0
  property bool fired: false
  interval: 100
  repeat: true
  triggeredOnStart: true
  onRunningChanged: {
    if(running){
      startTime = new Date().getTime()
    }
  }
  onTriggered: {
    var remainTime = countTime - (new Date().getTime() - startTime)
    remainTime = Math.min(countTime, remainTime)
    if(remainTime > 0){
      updateRemainTimeString(remainTime)
    }else{
      remainTimeString = "00:00"
      fired = true
    }
  }
  function updateRemainTimeString(time){
    var remain = new Date(time)
    remainTimeString = "%1:%2".arg(zeroPadding(remain.getMinutes()))
                        .arg(zeroPadding(remain.getSeconds()))
  }
  function zeroPadding(value){
    return ("0" + value).slice(-2)
  }
  function clear(){
    fired = false
    stop()
    updateRemainTimeString(countTime)
  }
  Component.onCompleted: clear()
}
