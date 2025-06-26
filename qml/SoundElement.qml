import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Rectangle {
    width: parent.width
    height: 60
    color: "#646446"
    radius: 10

    property string name
    property string path
    property string sequence

    signal playRequested()
    signal stopRequested()

    Text {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 20

        font.pixelSize: 25
        text: name
        color: "#ddd"

        MouseArea {
            id: mouseArea
            hoverEnabled: true
            anchors.fill: parent
        }

        ToolTip {
            font.pixelSize: 12
            text: path
            delay: 300
            visible: mouseArea.containsMouse
        }
    }

    CustomButton {
        id: stopButton
        anchors.right: parent.right
        anchors.rightMargin: 20

        text: "stop"
        onClicked: stopRequested()
    }
    CustomButton {
        id: playButton
        anchors.right: stopButton.left
        anchors.rightMargin: 10

        text: "play"
        onClicked: playRequested()
    }
}
