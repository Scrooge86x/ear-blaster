import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Window {
    id: root
    visible: true
    width: 900
    minimumWidth: 600
    height: 650
    minimumHeight: 400
    title: "ear-blaster"
    color: "#3c3c24"

    ListModel {
        id: soundConfigModel
        Component.onCompleted: {
            append({ name: "Sound 1", path: "E:\\test-sounds\\1.mp3", sequence: "Ctrl+Shift+1" })
            append({ name: "Sound 2", path: "E:\\test-sounds\\2.wav", sequence: "Ctrl+Shift+2" })
        }
    }

    property var audioDevices: []
    Component.onCompleted: {
        audioDevices = soundPlayer.getDevices();
        if (audioDevices.length) {
            deviceComboBox.currentIndex = 0;
            soundPlayer.setDevice(audioDevices[0].id);
        }
    }

    ComboBox  {
        id: deviceComboBox
        anchors {
            top: parent.top
            right: parent.right
            margins: 7
        }
        width: 300

        model: audioDevices
        textRole: "name"

        onActivated: (index) => {
            const device = audioDevices[index];
            soundPlayer.setDevice(device.id);
        }

        delegate: ItemDelegate {
            width: deviceComboBox.width
            text: modelData.name
            highlighted: deviceComboBox.highlightedIndex === index
        }
    }

    ColumnLayout {
        anchors {
            top: deviceComboBox.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            margins: 7
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            ScrollBar.vertical.width: 12
            ScrollBar.vertical.interactive: true

            padding: 0
            rightPadding: ScrollBar.vertical.width + 7

            ListView {
                model: soundConfigModel
                spacing: 3

                delegate: SoundElement {
                    name: model.name
                    path: model.path
                    sequence: model.sequence
                    onPlayRequested: {
                        soundPlayer.play(model.path)
                    }
                    onStopRequested: {
                        soundPlayer.stop()
                    }
                }
            }
        }
    }

    RoundButton {
        id: addSoundButton
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        font.pixelSize: 30

        onClicked: {
            const n = soundConfigModel.count + 1
            soundConfigModel.append({
                name: `Sound ${n}`,
                path: `E:\\test-sounds\\${n}.mp3`,
                sequence: `Ctrl+Shift+${n}`,
            });
        }

        width: 50
        height: 50
        scale: addSoundButton.down ? 0.96 : 1

        contentItem: Text {
            text: "+"
            font.family: "Arial"
            font.pixelSize: 30
            color: "#ddd"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            anchors.fill: parent
            color: "#646446"
            border.width: 1
            border.color: "#3c3c24"
            radius: addSoundButton.width / 2
        }
    }
}
