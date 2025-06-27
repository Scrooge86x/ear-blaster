import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Rectangle {
    id: root
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

    TextField {
        id: soundNameInput
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
            right: playButton.left
            rightMargin: 20
        }

        font.pixelSize: 20
        color: "#ddd"
        text: name

        onTextChanged: name = text

        background: Rectangle {
            anchors.fill: parent
            color: root.color
            border.width: soundNameInput.cursorVisible ? 1 : 0
            border.color: "#ddd"
            radius: 7
        }

        ToolTip {
            y: 0
            font.pixelSize: 12
            text: path
            delay: 300
            visible: soundNameInput.hovered
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
