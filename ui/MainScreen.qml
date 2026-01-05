import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal
import QtQuick.Dialogs
import QtMultimedia
import QtTextToSpeech
import ui.settings
import ui.components

Item {
    id: root

    signal settingsClicked()

    function addSoundFile(fileUrl) {
        let filePath = fileUrl.toString().replace("file://", "")

        // Remove leading slash on windows so the paths are [A-Z]:/* instead of /[A-Z]:/*
        if (Qt.platform.os === "windows") {
            filePath = filePath.substring(1);
        }

        const lastSlashPos = filePath.lastIndexOf("/");
        const lastDotPos = filePath.lastIndexOf(".");
        soundListModel.append({
            name: filePath.substring(lastSlashPos + 1, lastDotPos),
            path: filePath,
            sequence: "",
        });
    }

    property mediaFormat mediaFormatInfo
    property list<string> supportedExtensions
    property list<string> supportedExtensionFilters

    Component.onCompleted: {
        const extensionMap = {
            [MediaFormat.AudioCodec.MP3]: "mp3",
            [MediaFormat.AudioCodec.AAC]: "aac",
            [MediaFormat.AudioCodec.AC3]: "ac3",
            [MediaFormat.AudioCodec.EAC3]: "eac3",
            [MediaFormat.AudioCodec.FLAC]: "flac",
            [MediaFormat.AudioCodec.DolbyTrueHD]: "thd",
            [MediaFormat.AudioCodec.Opus]: "opus",
            [MediaFormat.AudioCodec.Vorbis]: "ogg",
            [MediaFormat.AudioCodec.Wave]: "wav",
            [MediaFormat.AudioCodec.WMA]: "wma",
            [MediaFormat.AudioCodec.ALAC]: "alac",
        };

        const extensions = [];
        const extensionFilters = [];

        for (const format of root.mediaFormatInfo.supportedAudioCodecs(MediaFormat.Decode)) {
            const extension = extensionMap[format];
            extensions.push(extension);
            extensionFilters.push(`${root.mediaFormatInfo.audioCodecDescription(format)} (*.${extension})`);
        }
        extensionFilters.unshift(`Audio (${extensions.map(format => `*.${format}`).join(" ")})`);

        root.supportedExtensions = extensions;
        root.supportedExtensionFilters = extensionFilters;
    }

    RowLayout {
        id: topBar
        anchors {
            top: root.top
            left: root.left
            right: root.right
            leftMargin: 7
        }

        VolumeInput {
            text: qsTr("Volume:")
            sliderWidth: 100
            value: AppSettings.outputVolume
            onValueChanged: AppSettings.outputVolume = value
        }

        VolumeInput {
            Layout.leftMargin: 5
            text: qsTr("Overdrive:")
            sliderWidth: 75
            value: AppSettings.outputOverdrive
            onValueChanged: AppSettings.outputOverdrive = value
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            checkable: true
            icon.source: `qrc:/qt/qml/ui/resources/pictogrammers/microphone${checked ? "" : "-off"}.svg`
            checked: AppSettings.micPassthroughEnabled
            onToggled: AppSettings.micPassthroughEnabled = checked
        }

        Button {
            checkable: true
            icon.source: `qrc:/qt/qml/ui/resources/pictogrammers/headphones${checked ? "" : "-off"}.svg`
            checked: AppSettings.audioMonitorEnabled
            onToggled: AppSettings.audioMonitorEnabled = checked
        }

        Button {
            text: qsTr("Stop all")
            onClicked: audioSystem.stopAll()
        }

        AudioDeviceSelect {
            Layout.preferredWidth: 250
            appSettingsPropName: "audioOutputDevice"
            deviceType: AudioDeviceSelect.DeviceType.Output
        }

        Button {
            icon.source: "qrc:/qt/qml/ui/resources/pictogrammers/cog.svg"
            onClicked: root.settingsClicked()
        }
    }

    ListModel {
        id: soundListModel
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
                    soundListModel.append(sound);
                }
            } catch (error) {
                console.error("Failed parsing config file: ", error);
            }
        }
        Component.onDestruction: {
            const sounds = [];
            for (let i = 0; i < soundListModel.count; ++i) {

                // Cannot directly push sound, cause it also includes other keys
                const sound = soundListModel.get(i);
                sounds.push({
                    name: sound.name,
                    path: sound.path,
                    sequence: sound.sequence,
                });
            }

            AppSettings.sounds = JSON.stringify(sounds);
            console.info("Config written to:", AppSettings.location);
        }
    }

    SoundList {
        id: soundList
        listModel: soundListModel
        anchors {
            top: topBar.bottom
            right: root.right
            bottom: ttsContainer.top
            left: root.left
            topMargin: 7
            bottomMargin: 7
        }
    }

    Rectangle {
        id: ttsContainer

        color: AppSettings.backgroundColor
        height: 50
        anchors {
            left: root.left
            right: root.right
            bottom: root.bottom
        }

        RowLayout {
            anchors.fill: parent

            TextField {
                id: ttsText

                Layout.fillWidth: true
                Layout.fillHeight: true
                color: AppSettings.foregroundColor
                placeholderTextColor: Qt.tint(AppSettings.foregroundColor, Qt.rgba(0, 0, 0, 0.3))
                placeholderText: qsTr("Text to speech...")

                onAccepted: {
                    audioSystem.tts.say(text);
                    text = "";
                }

                background: Rectangle {
                    border.color: AppSettings.foregroundColor
                    color: AppSettings.backgroundColor
                    radius: 7
                }
            }

            RoundButton {
                id: ttsButton

                property string untranslatedText

                text: qsTr(untranslatedText)
                Layout.fillHeight: true
                radius: 7
                onClicked: {
                    if (audioSystem.tts.isPlaying()) {
                        audioSystem.tts.stop();
                    } else {
                        audioSystem.tts.say(ttsText.text);
                    }
                }

                Connections {
                    target: audioSystem

                    function onTtsStarted() {
                        ttsButton.untranslatedText = QT_TR_NOOP("Stop TTS");
                    }
                    function onTtsStopped() {
                        ttsButton.untranslatedText = QT_TR_NOOP("Play TTS");
                    }
                }
            }
        }
    }

    RoundButton {
        id: addSoundButton
        font.pixelSize: 30
        width: 50
        height: 50
        scale: addSoundButton.down ? 0.96 : 1

        anchors {
            right: soundList.right
            rightMargin: 25
            bottom: soundList.bottom
            bottomMargin: 10
        }
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

        onClicked: fileDialog.open()

        FileDialog {
            id: fileDialog
            acceptLabel: qsTr("Select")
            rejectLabel: qsTr("Cancel")
            fileMode: FileDialog.OpenFiles
            nameFilters: root.supportedExtensionFilters
            onAccepted: {
                for (const file of selectedFiles) {
                    root.addSoundFile(file);
                }
            }
        }
    }

    Rectangle {
        anchors.fill: root
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
        enabled: !soundList.stealingFocus

        property list<string> acceptedUrls: []

        anchors.fill: root
        keys: ["text/uri-list"]

        onEntered: (drag) => {
            if (!drag.hasUrls) {
                return;
            }

            const allowedExtensions = root.supportedExtensions;
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
                root.addSoundFile(url);
            }
            acceptedUrls = [];
        }
    }
}
