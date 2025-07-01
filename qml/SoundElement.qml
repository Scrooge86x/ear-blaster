import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Rectangle {
    id: root
    width: parent?.width // ?. because of "Cannot read property 'width' of null" when removing the sound
    height: 60
    color: Qt.lighter(Universal.background, 2.2)
    radius: 10

    property string name
    property string path
    property string sequence

    signal playRequested()
    signal stopRequested()
    signal deleteRequested()

    Text {
        id: tripleBar
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 7

        font.pixelSize: 20
        color: Universal.foreground
        text: "≡"
        font.family: "Arial"
    }

    TextField {
        id: soundNameInput
        anchors {
            verticalCenter: parent.verticalCenter
            left: tripleBar.right
            leftMargin: 6
            right: playButton.left
            rightMargin: 10
        }

        font.pixelSize: 20
        color: Universal.foreground
        text: name

        onTextChanged: name = text
        onEditingFinished: focus = false
        onFocusChanged: root.focusChanged(focus)

        leftPadding: 6
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
        anchors.right: sequenceInput.left
        anchors.rightMargin: 10
        leftPadding: 15
        rightPadding: 15

        radius: 7
        text: qsTr("delete")
        onClicked: deleteRequested()
    }

    SequenceInput {
        id: sequenceInput
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10
        width: 150
        height: 36
        sequence: root.sequence

        onFocusChanged: root.focusChanged(focus)
        onEditingFinished: focus = false
        onSequenceChanged: root.sequence = sequenceInput.sequence
    }

    Connections {
        enabled: sequence !== ""
        target: globalKeyListener
        function onGlobalHotkeyPressed(hotkey) {
            if (hotkey === sequence) {
                playRequested();
            }
        }
    }
}
