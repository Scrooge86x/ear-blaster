import QtQuick
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

        CheckBox {
            text: qsTr("Monitor")
            checked: AppSettings.audioMonitorEnabled
            onToggled: AppSettings.audioMonitorEnabled = checked
        }

        Item { Layout.fillWidth: true }

        RoundButton {
            text: qsTr("Stop all")
            radius: 7
            onClicked: audioSystem.stopAll()
        }

        AudioDeviceSelect {
            Layout.preferredWidth: 250
            appSettingsPropName: "audioOutputDevice"
            deviceType: AudioDeviceSelect.DeviceType.Output
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
                    addSoundFile(file);
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: fileDropArea.acceptedUrls.length ? Qt.rgba(0, 0, 0, 0.5) : "transparent"

        Text {
            property alias acceptedUrls: fileDropArea.acceptedUrls

            anchors.centerIn: parent
            font.pixelSize: 40
            color: acceptedUrls.length ? "#ddd" : "transparent"
            text: qsTr("Drop %n file(s).", "", acceptedUrls.length)
        }
    }

    DropArea {
        id: fileDropArea
        anchors.fill: parent
        keys: ["text/uri-list"]

        property list<string> acceptedUrls: []

        onEntered: (drag) => {
            if (!drag.hasUrls) {
                return;
            }

            const allowedExtensions = ["mp3", "wav"];

            for (let url of drag.urls) {
                url = url.toString();
                const lastDotPos = url.lastIndexOf(".");
                const extension = url.substring(lastDotPos + 1).toLowerCase();
                if (allowedExtensions.includes(extension)) {
                    acceptedUrls.push(url);
                }
            }

            if (acceptedUrls.length > 0) {
                drag.accept();
            }
        }

        onExited: {
            acceptedUrls = [];
        }

        onDropped: (drop) => {
            for (const url of acceptedUrls) {
                addSoundFile(url);
            }
            acceptedUrls = [];
        }
    }

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

    function addSoundFile(fileUrl) {
        let filePath = fileUrl.toString().replace("file://", "")

        // Remove leading slash on windows so the paths are [A-Z]:/* instead of /[A-Z]:/*
        if (Qt.platform.os === "windows") {
            filePath = filePath.substring(1);
        }

        const lastSlashPos = filePath.lastIndexOf("/");
        const lastDotPos = filePath.lastIndexOf(".");
        soundConfigModel.append({
            name: filePath.substring(lastSlashPos + 1, lastDotPos),
            path: filePath,
            sequence: "",
        });
    }
}
