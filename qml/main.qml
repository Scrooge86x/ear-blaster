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

    property var soundConfig: [
        {
            name: "Sound 1",
            path: "E:\\test-sounds\\1.mp3",
            sequence: "Ctrl+Shift+1"
        },
        {
            name: "Sound 2",
            path: "E:\\test-sounds\\2.wav",
            sequence: "Ctrl+Shift+2"
        }
    ]

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 7

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                model: soundConfig
                spacing: 3

                delegate: SoundElement {
                    name: modelData.name
                    path: modelData.path
                    sequence: modelData.sequence
                    onPlayRequested: {
                        soundPlayer.play(modelData.path)
                    }
                }
            }
        }
    }
}
