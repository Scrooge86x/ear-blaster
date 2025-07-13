import QtQuick
import QtMultimedia
import QtQuick.Layouts
import QtQuick.Controls.Universal
import QtQuick.Dialogs

Item {
    signal settingsClicked()

    property ListModel audioDevices: ListModel {}

    RowLayout {
        id: topBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        VolumeInput {
            text: qsTr("Volume:")
            value: AppSettings.outputVolume
            onValueChanged: AppSettings.outputVolume = value
            sliderWidth: 100
        }

        VolumeInput {
            text: qsTr("Overdrive:")
            value: AppSettings.outputOverdrive
            onValueChanged: AppSettings.outputOverdrive = value
            sliderWidth: 75
        }

        CheckBox {
            text: qsTr("Mic")
            checked: AppSettings.micPassthroughEnabled
            onToggled: AppSettings.micPassthroughEnabled = checked
        }

        Item { Layout.fillWidth: true }

        RoundButton {
            text: qsTr("Stop all")
            radius: 7
            onClicked: audioSystem.stopAll()
        }

        ComboBox  {
            id: deviceComboBox
            model: audioSystem.audioOutputs
            valueRole: "id"
            textRole: "description"
            Layout.preferredWidth: 250
            displayText: currentText.length ? currentText : qsTr("No device selected.")

            delegate: ItemDelegate {
                width: ListView.view.width
                text: modelData.description.length ? modelData.description : qsTr("No device selected.")
                font.weight: deviceComboBox.currentIndex === index ? Font.DemiBold : Font.Normal
                highlighted: deviceComboBox.highlightedIndex === index
                hoverEnabled: deviceComboBox.hoverEnabled
            }

            onCurrentIndexChanged: {
                const wasDeviceDisconnected = currentIndex === 0 && highlightedIndex === -1;
                if (wasDeviceDisconnected) {
                    currentIndex = audioSystem.getOutputDeviceIndexById(AppSettings.audioOutputDevice);
                    AppSettings.audioOutputDevice = valueAt(currentIndex);
                }
            }
            onActivated: (index) => AppSettings.audioOutputDevice = valueAt(index)

            // Cannot use indexOfValue here
            Component.onCompleted: currentIndex = audioSystem.getOutputDeviceIndexById(AppSettings.audioOutputDevice)
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
