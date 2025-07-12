import QtQuick
import QtMultimedia
import QtQuick.Layouts
import QtQuick.Controls.Universal
import QtQuick.Dialogs

Item {
    signal settingsClicked()

    property ListModel audioDevices: ListModel {}

    Component.onCompleted: {
        const outputDevice = audioSystem.getOutputDeviceById(AppSettings.audioOutputDevice);
        if (outputDevice.mode === AudioDevice.Null) {
            console.error("Warning: audio output device not found:", AppSettings.audioOutputDevice);
            audioSystem.outputDevice.device = mediaDevices.defaultAudioOutput;
        } else {
            audioSystem.outputDevice.device = outputDevice;
        }

        audioSystem.micPassthrough.inputDevice.device = mediaDevices.defaultAudioInput;
    }

    MediaDevices {
        id: mediaDevices
        onAudioOutputsChanged: deviceComboBox.currentIndex = audioSystem.getOutputDeviceIndexById(AppSettings.audioOutputDevice)
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
                value: AppSettings.outputVolume
                stepSize: 0.01
                Layout.preferredWidth: 100

                onValueChanged: AppSettings.outputVolume = value
            }

            Label {
                Layout.preferredWidth: 35
                text: `${Math.round(volumeSlider.value * 100)}%`
                color: AppSettings.foregroundColor
                font.pixelSize: 13
            }
        }

        RowLayout {
            Label {
                text: qsTr("Overdrive:")
                color: AppSettings.foregroundColor
            }

            Slider {
                id: overdriveSlider
                from: 0.0
                to: 1.0
                value: AppSettings.overdrive
                stepSize: 0.01
                Layout.preferredWidth: 75

                onValueChanged: AppSettings.overdrive = value
            }

            Label {
                Layout.preferredWidth: 35
                text: `${Math.round(overdriveSlider.value * 100)}%`
                color: AppSettings.foregroundColor
                font.pixelSize: 13
            }
        }

        CheckBox {
            text: qsTr("Mic")
            checked: AppSettings.micPassthrough
            onToggled: AppSettings.micPassthrough = checked
        }

        Item { Layout.fillWidth: true }

        RoundButton {
            text: qsTr("Stop all")
            radius: 7
            onClicked: audioSystem.stopAll()
        }

        ComboBox  {
            id: deviceComboBox
            model: mediaDevices.audioOutputs
            currentIndex: audioSystem.getOutputDeviceIndexById(AppSettings.audioOutputDevice)
            valueRole: "id"
            textRole: "description"
            Layout.preferredWidth: 250

            onActivated: AppSettings.audioOutputDevice = currentValue
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
}
