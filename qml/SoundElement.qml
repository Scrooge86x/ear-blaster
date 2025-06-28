import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Rectangle {
    id: root
    width: parent?.width // ?. because of "Cannot read property 'width' of null" when removing the sound
    height: 60
    color: Universal.accent
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
        anchors.leftMargin: 10

        font.pixelSize: 20
        color: Universal.foreground
        text: "≡"
        font.family: "Arial"
    }

    TextField {
        id: soundNameInput
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
            right: playButton.left
            rightMargin: 20
        }

        Universal.accent: Universal.Cobalt

        font.pixelSize: 20
        color: Universal.foreground
        text: name

        onTextChanged: name = text
        onEditingFinished: soundNameInput.focus = false
        onFocusChanged: root.focusChanged(focus)

        background: Rectangle {
            anchors.fill: parent
            color: root.color
            border.width: soundNameInput.focus ? 1 : 0
            border.color: Universal.foreground
            radius: 7
        }

        ToolTip {
            x: 0
            y: -20
            font.pixelSize: 12
            text: path
            delay: 400
            visible: !soundNameInput.focus && soundNameInput.hovered
        }
    }

    RoundButton {
        id: playButton
        Universal.foreground: "#fff"
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: stopButton.left
        anchors.rightMargin: 10
        leftPadding: 15
        rightPadding: 15

        radius: 7
        text: qsTr("play")
        onClicked: playRequested()
    }

    RoundButton {
        id: stopButton
        Universal.foreground: "#fff"
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: deleteButton.left
        anchors.rightMargin: 10
        leftPadding: 15
        rightPadding: 15

        radius: 7
        text: qsTr("stop")
        onClicked: stopRequested()
    }

    RoundButton {
        id: deleteButton
        Universal.foreground: "#fff"
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10
        leftPadding: 15
        rightPadding: 15

        radius: 7
        text: qsTr("delete")
        onClicked: deleteRequested()
    }
}
