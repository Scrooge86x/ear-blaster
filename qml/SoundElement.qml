import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Rectangle {
    width: parent?.width // ?. because of "Cannot read property 'width' of null" when removing the sound
    height: 60
    color: "#646446"
    radius: 10

    property string name
    property string path
    property string sequence

    signal playRequested()
    signal stopRequested()
    signal deleteRequested()

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
        id: playButton
        anchors.right: stopButton.left
        anchors.rightMargin: 10

        text: qsTr("play")
        onClicked: playRequested()
    }

    CustomButton {
        id: stopButton
        anchors.right: deleteButton.left
        anchors.rightMargin: 10

        text: qsTr("stop")
        onClicked: stopRequested()
    }

    CustomButton {
        id: deleteButton
        anchors.right: parent.right
        anchors.rightMargin: 10

        text: qsTr("delete")
        onClicked: deleteRequested()
    }
}
