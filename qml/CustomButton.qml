import QtQuick
import QtQuick.Controls.Universal

RoundButton {
    id: root
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.rightMargin: 30
    scale: root.down ? 0.96 : 1

    contentItem: Text {
        text: root.text
        font: root.font
        color: "#ddd"
        anchors.centerIn: buttonBackground
    }

    background: Rectangle {
        id: buttonBackground
        width: root.width + 15
        height: root.height
        color: "#3c3c24"
        border.color: "#3c3c24"
        border.width: 1
        radius: 7
    }
}
