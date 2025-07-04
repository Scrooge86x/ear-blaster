import QtQuick
import QtMultimedia
import QtQuick.Layouts
import QtQuick.Controls.Universal
import QtQuick.Dialogs

Item {
    signal settingsClicked()

    property ListModel audioDevices: ListModel {}

    Connections {
        target: audioSystem
        function onOutputDeviceChanged(outputDevice) {
            AppSettings.audioOutputDevice = outputDevice.id;
        }
    }

    Component.onCompleted: {
        let deviceFound = false;
        for (const device of mediaDevices.audioOutputs) {
            if (device.id.toString() === AppSettings.audioOutputDevice) {
                audioSystem.outputDevice = device;
                deviceFound = true;
            }
        }
        if (!deviceFound) {
            console.error("Warning: audio output device not found:", AppSettings.audioOutputDevice);
            audioSystem.outputDevice = mediaDevices.audioOutputs[0];
        }
        updateAudioDevices();
    }

    MediaDevices {
        id: mediaDevices
        onAudioOutputsChanged: updateAudioDevices()
    }

    RowLayout {
        id: topBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        RowLayout {
            Label {
                text: qsTr("Volume:")
                color: AppSettings.foregroundColor
            }

            Slider {
                id: volumeSlider
                from: 0.0
                to: 1.0
                value: AppSettings.mainVolume
                stepSize: 0.01
                Layout.preferredWidth: 150

                onMoved: AppSettings.mainVolume = value
            }

            Label {
                Layout.preferredWidth: 35
                text: `${Math.round(volumeSlider.value * 100)}%`
                color: AppSettings.foregroundColor
                font.pixelSize: 13
            }
        }

        Item { Layout.fillWidth: true }

        ComboBox  {
            id: deviceComboBox
            model: audioDevices
            textRole: "name"
            Layout.preferredWidth: 300

            onActivated: (index) => audioSystem.outputDevice = mediaDevices.audioOutputs[index]
            delegate: ItemDelegate {
                width: deviceComboBox.width
                text: model.name
                highlighted: deviceComboBox.highlightedIndex === index
            }
        }

        Button {
            text: qsTr("Settings")
            onClicked: settingsClicked()
        }
    }

    ListModel {
        id: soundConfigModel
        Component.onCompleted: {
            try {
                const sounds = JSON.parse(AppSettings.sounds);
                if (!Array.isArray(sounds)) {
                    return console.error("Error: corrupted sounds list in config file.");
                }

                for (const sound of sounds) {
                    const expectedKeys = ["name", "path", "sequence"];
                    if (Object.keys(sound).length !== expectedKeys.length
                            || !expectedKeys.every(key => key in sound)) {
                        console.error("Error: corruted sound detected in config file.", JSON.stringify(sound));
                        continue;
                    }
                    append(sound);
                }
            } catch (error) {
                console.error("Failed parsing config file: ", error);
            }
        }
        Component.onDestruction: {
            const soundList = [];
            for (let i = 0; i < soundConfigModel.count; ++i) {

                // Cannot directly push sound, cause it also includes other keys
                const sound = soundConfigModel.get(i);
                soundList.push({
                    name: sound.name,
                    path: sound.path,
                    sequence: sound.sequence,
                });
            }

            AppSettings.sounds = JSON.stringify(soundList);
            console.info("Config written to:", AppSettings.location);
        }
    }

    SoundList {
        listModel: soundConfigModel
        anchors {
            top: topBar.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            topMargin: 7
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
            color: AppSettings.foregroundColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            anchors.fill: parent
            color: AppSettings.accentColor
            radius: addSoundButton.width / 2
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
    }

    function updateAudioDevices() {
        audioDevices.clear();
        for (let i = 0; i < mediaDevices.audioOutputs.length; ++i) {
            const device = mediaDevices.audioOutputs[i];
            audioDevices.append({ name: device.description });
            if (device.id.toString() === AppSettings.audioOutputDevice) {
                deviceComboBox.currentIndex = i
            }
        }
    }
}
