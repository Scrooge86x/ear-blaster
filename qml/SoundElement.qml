import QtQuick
import QtQuick.Controls.Universal

Rectangle {
    id: root

    property string name
    property string path
    property string sequence
    property int initialIndex
    property bool disablePlayback: false
    property bool isPlaying: false

    signal deleteRequested()

    width: parent?.width // ?. because of "Cannot read property 'width' of null" when removing the sound
    height: 60
    radius: 10
    color: Qt.tint(AppSettings.backgroundColor, Qt.rgba(255, 255, 255, isPlaying ? 0.15 : 0.075))

    Text {
        id: tripleBar
        font.pixelSize: 20
        color: AppSettings.foregroundColor
        text: "≡"
        font.family: "Arial"

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 7
        }
    }

    TextField {
        id: soundNameInput
        font.pixelSize: 20
        color: AppSettings.foregroundColor
        text: name
        leftPadding: 6

        anchors {
            verticalCenter: parent.verticalCenter
            left: tripleBar.right
            leftMargin: 6
            right: playButton.left
            rightMargin: 10
        }
        background: Rectangle {
            anchors.fill: parent
            color: root.color
            radius: 7
            border {
                width: soundNameInput.focus ? 1 : 0
                color: AppSettings.foregroundColor
            }
        }

        onTextChanged: name = text
        onEditingFinished: focus = false
        onFocusChanged: root.focusChanged(focus)

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
        text: qsTr("play")
        Universal.foreground: "#fff"
        radius: 7
        leftPadding: 15
        rightPadding: 15

        anchors {
            verticalCenter: parent.verticalCenter
            right: stopButton.left
            rightMargin: 10
        }

        onClicked: audioSystem.play(initialIndex, path)
    }

    RoundButton {
        id: stopButton
        text: qsTr("stop")
        Universal.foreground: "#fff"
        leftPadding: 15
        rightPadding: 15
        radius: 7

        anchors {
            verticalCenter: parent.verticalCenter
            right: deleteButton.left
            rightMargin: 10
        }

        onClicked: audioSystem.stop(initialIndex)
    }

    RoundButton {
        id: deleteButton
        text: qsTr("delete")
        Universal.foreground: "#fff"
        leftPadding: 15
        rightPadding: 15
        radius: 7

        anchors {
            verticalCenter: parent.verticalCenter
            right: sequenceInput.left
            rightMargin: 10
        }

        onClicked: {
            audioSystem.stop(initialIndex);
            deleteRequested();
        }
    }

    SequenceInput {
        id: sequenceInput
        width: 150
        height: 36
        sequence: root.sequence
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 10
        }

        onFocusChanged: root.focusChanged(focus)
        onEditingFinished: focus = false
        onSequenceChanged: root.sequence = sequenceInput.sequence
    }

    Connections {
        target: audioSystem

        function onSoundStarted(id) {
            if (id === initialIndex) {
                root.isPlaying = true;
            }
        }
        function onSoundStopped(id) {
            if (id === initialIndex) {
                root.isPlaying = false;
            }
        }
    }

    Connections {
        enabled: sequence !== ""
        target: globalKeyListener

        function onCurrentSequenceChanged(hotkey) {
            if (!disablePlayback && hotkey === sequence) {
                switch (AppSettings.secondPressBehavior) {
                case AppSettings.SecondPressBehavior.StartOver:
                    audioSystem.play(initialIndex, path);
                    break;
                case AppSettings.SecondPressBehavior.StopSound:
                    if (root.isPlaying) {
                        audioSystem.stop(initialIndex);
                    } else {
                        audioSystem.play(initialIndex, path);
                    }
                    break;
                default:
                    console.error("Invalid second press behavior specified:", AppSettings.secondPressBehavior);
                    break;
                }
            }
        }
    }
}
