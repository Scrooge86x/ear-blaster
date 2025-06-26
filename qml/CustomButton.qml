import QtQuick
import QtQuick.Controls.Universal

RoundButton {
    id: root
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.rightMargin: 30

    scale: root.down ? 0.96 : 1
    leftPadding: 20
    rightPadding: 20

    contentItem: Text {
        text: root.text
        font: root.font
        color: "#ddd"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        anchors.fill: parent
        color: "#3c3c24"
        radius: 7
    }
}
