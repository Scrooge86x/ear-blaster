import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal
import QtQuick.Dialogs
import QtMultimedia

Window {
    id: root
    visible: true
    width: 900
    minimumWidth: 600
    height: 650
    minimumHeight: 400
    title: "Ear Blaster"
    color: Universal.background

    Universal.theme: Universal.Dark
    Universal.foreground: "#ddd"
    Universal.background: "#0f0f0f"
    Universal.accent: Universal.Orange

    ListModel {
        id: soundConfigModel
        Component.onCompleted: {
            append({ name: "Sound 1", path: "E:\\test-sounds\\1.mp3", sequence: "Ctrl+Num1" });
            append({ name: "Sound 2", path: "E:\\test-sounds\\2.wav", sequence: "Ctrl+Num2" });
        }
    }

    property ListModel audioDevices: ListModel {}
    function updateAudioDevices() {
        audioDevices.clear();
        for (let i = 0; i < mediaDevices.audioOutputs.length; ++i) {
            const device = mediaDevices.audioOutputs[i];
            audioDevices.append({ name: device.description });
            if (device.id.toString() === soundPlayer.getDevice().id.toString()) {
                deviceComboBox.currentIndex = i
            }
        }
    }

    MediaDevices {
        id: mediaDevices
        onAudioOutputsChanged: updateAudioDevices()
    }

    Component.onCompleted: {
        deviceComboBox.currentIndex = 0;
        soundPlayer.setDevice(mediaDevices.audioOutputs[0]);
        updateAudioDevices();
    }

    RowLayout {
        id: topBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 7
        }

        RowLayout {
            Label {
                text: qsTr("Volume:")
                color: Universal.foreground
            }

            Slider {
                id: volumeSlider
                from: 0.0
                to: 1.0
                value: 1.0
                stepSize: 0.01
                Layout.preferredWidth: 150

                onMoved: soundPlayer.setVolume(value)
            }

            Label {
                Layout.preferredWidth: 35
                text: `${Math.round(volumeSlider.value * 100)}%`
                color: Universal.foreground
                font.pixelSize: 13
            }
        }

        Item { Layout.fillWidth: true }

        ComboBox  {
            id: deviceComboBox
            model: audioDevices
            textRole: "name"
            Layout.preferredWidth: 300

            onActivated: (index) => soundPlayer.setDevice(mediaDevices.audioOutputs[index])
            delegate: ItemDelegate {
                width: deviceComboBox.width
                text: model.name
                highlighted: deviceComboBox.highlightedIndex === index
            }
        }
    }

    SoundList {
        listModel: soundConfigModel
        anchors {
            top: topBar.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            margins: 7
        }
    }

    FileDialog {
        id: fileDialog
        acceptLabel: qsTr("Select")
        rejectLabel: qsTr("Cancel")
        fileMode: FileDialog.OpenFiles
        nameFilters: [
            "Audio (*.mp3 *.wav)",
            "MP3 (*.mp3)",
            "WAV (*.wav)",
        ]
        onAccepted: {
            for (const file of selectedFiles) {
                let filePath = file.toString().replace("file://", "")
                if (Qt.platform.os === "windows") {
                    filePath = filePath.substring(1);
                }

                const slashPos = filePath.lastIndexOf("/");
                const dotPos = filePath.lastIndexOf(".");
                soundConfigModel.append({
                    name: filePath.substring(slashPos + 1, dotPos),
                    path: filePath,
                    sequence: "",
                });
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

        onClicked: fileDialog.open()

        width: 50
        height: 50
        scale: addSoundButton.down ? 0.96 : 1

        contentItem: Text {
            text: "+"
            font.family: "Arial"
            font.pixelSize: 30
            color: Universal.foreground
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            anchors.fill: parent
            color: Universal.accent
            radius: addSoundButton.width / 2
        }
    }
}
