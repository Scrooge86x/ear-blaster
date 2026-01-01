pragma Singleton

import QtCore

Settings {
    id: appSettings
    location: StandardPaths.writableLocation(StandardPaths.ConfigLocation) + "/config.ini"

    // If this was an object instead of a function it would get saved
    // in the config file
    function getDefaults() {
        return {
            outputVolume: 1.0,
            inputVolume: 1.0,
            monitorVolume: 1.0,
            outputOverdrive: 0.0,
            inputOverdrive: 0.0,
            monitorOverdrive: 0.0,
            foregroundColor: "#ddd",
            backgroundColor: "#0f0f0f",
            accentColor: "#fa6800",
            closeBehavior: AppSettings.CloseBehavior.HideKeepTray,
            secondPressBehavior: AppSettings.SecondPressBehavior.StopSound,
            windowGeometry: "{}", // Anything other than "" will get filled in during shutdown
            micPassthroughEnabled: false,
            audioMonitorEnabled: false,
            audioMonitorMatchOutput: true,
        };
    }

    property string sounds: "[]" // { "name": "", "path": "", "sequence": "" }
    property real outputVolume: getDefaults()["outputVolume"]
    property real inputVolume: getDefaults()["inputVolume"]
    property real monitorVolume: getDefaults()["monitorVolume"]
    property real outputOverdrive: getDefaults()["outputOverdrive"]
    property real inputOverdrive: getDefaults()["inputOverdrive"]
    property real monitorOverdrive: getDefaults()["monitorOverdrive"]
    property string foregroundColor: getDefaults()["foregroundColor"]
    property string backgroundColor: getDefaults()["backgroundColor"]
    property string accentColor: getDefaults()["accentColor"]
    property string windowGeometry: getDefaults()["windowGeometry"] // { "width": 0, "height": 0, "x": 0, "y": 0, "maximized": false }
    property string language: "en"
    property string audioOutputDevice: ""
    property string audioInputDevice: ""
    property string audioMonitorDevice: ""
    property bool micPassthroughEnabled: getDefaults()["micPassthroughEnabled"]
    property bool audioMonitorEnabled: getDefaults()["audioMonitorEnabled"]
    property bool audioMonitorMatchOutput: getDefaults()["audioMonitorMatchOutput"]
    property string ttsLocale: " " // Space is there so onTtsLocaleChanged always gets called
    property string ttsVoice: " "

    onOutputVolumeChanged: {
        audioSystem.outputDevice.volume = outputVolume;
        if (audioMonitorMatchOutput) {
            audioSystem.monitorDevice.volume = outputVolume;
        }
    }
    onInputVolumeChanged: audioSystem.micPassthrough.inputDevice.volume = inputVolume
    onMonitorVolumeChanged: if (!audioMonitorMatchOutput) audioSystem.monitorDevice.volume = monitorVolume
    onOutputOverdriveChanged: {
        audioSystem.outputDevice.overdrive = outputOverdrive;
        if (audioMonitorMatchOutput) {
            audioSystem.monitorDevice.overdrive = outputOverdrive;
        }
    }
    onInputOverdriveChanged: audioSystem.micPassthrough.inputDevice.overdrive = inputOverdrive
    onMonitorOverdriveChanged: if (!audioMonitorMatchOutput) audioSystem.monitorDevice.overdrive = monitorOverdrive
    onLanguageChanged: translator.currentLanguage = language
    onAudioOutputDeviceChanged: audioSystem.outputDevice.device = audioSystem.getOutputDeviceById(audioOutputDevice)
    onAudioInputDeviceChanged: audioSystem.micPassthrough.inputDevice.device = audioSystem.getInputDeviceById(audioInputDevice)
    onAudioMonitorDeviceChanged: audioSystem.monitorDevice.device = audioSystem.getOutputDeviceById(audioMonitorDevice)
    onMicPassthroughEnabledChanged: audioSystem.micPassthrough.inputDevice.enabled = micPassthroughEnabled
    onAudioMonitorEnabledChanged: audioSystem.monitorDevice.enabled = audioMonitorEnabled
    onAudioMonitorMatchOutputChanged: {
        if (audioMonitorMatchOutput) {
            audioSystem.monitorDevice.volume = outputVolume;
            audioSystem.monitorDevice.overdrive = outputOverdrive;
        } else {
            audioSystem.monitorDevice.volume = monitorVolume;
            audioSystem.monitorDevice.overdrive = monitorOverdrive;
        }
    }
    onTtsLocaleChanged: {
        audioSystem.tts.locale = Qt.locale(ttsLocale)
        ttsLocale = audioSystem.tts.locale.name
    }
    onTtsVoiceChanged: {
        for (const voice of audioSystem.tts.availableVoices()) {
            if (voice.name === ttsVoice) {
                audioSystem.tts.voice = voice;
            }
        }
        ttsVoice = audioSystem.tts.voice.name;
    }

    enum CloseBehavior {
        Quit,
        HideToTray,
        HideKeepTray
    }
    property real closeBehavior: getDefaults()["closeBehavior"]

    enum SecondPressBehavior {
        StartOver,
        StopSound
    }
    property real secondPressBehavior: getDefaults()["secondPressBehavior"]

    function restoreDefaults() {
        const defaults = getDefaults();
        for (const key in defaults) {
            appSettings[key] = defaults[key];
        }
    }
}
