import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: "ear-blaster"
    color: "#3c3c24"

    ListModel {
        id: soundConfigModel
        Component.onCompleted: {
            append({ name: "Sound 1", path: "E:\\test-sounds\\1.mp3", sequence: "Ctrl+Shift+1" })
            append({ name: "Sound 2", path: "E:\\test-sounds\\2.wav", sequence: "Ctrl+Shift+2" })
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 7

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

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
        anchors.rightMargin: 10
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
            radius: addSoundButton.width / 2
        }
    }
}
