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
            outputOverdrive: 0.0,
            inputOverdrive: 0.0,
            foregroundColor: "#ddd",
            backgroundColor: "#0f0f0f",
            accentColor: "#fa6800",
            closeBehavior: AppSettings.CloseBehavior.HideKeepTray,
            secondPressBehavior: AppSettings.SecondPressBehavior.StopSound,
            windowGeometry: "{}", // Anything other than "" will get filled in during shutdown
            language: "en",
            audioOutputDevice: "",
            audioInputDevice: "",
            micPassthroughEnabled: false,
        };
    }

    property string sounds: "[]" // { "name": "", "path": "", "sequence": "" }
    property real outputVolume: getDefaults()["outputVolume"]
    property real inputVolume: getDefaults()["inputVolume"]
    property real outputOverdrive: getDefaults()["outputOverdrive"]
    property real inputOverdrive: getDefaults()["inputOverdrive"]
    property string foregroundColor: getDefaults()["foregroundColor"]
    property string backgroundColor: getDefaults()["backgroundColor"]
    property string accentColor: getDefaults()["accentColor"]
    property string windowGeometry: getDefaults()["windowGeometry"] // { "width": 0, "height": 0, "x": 0, "y": 0, "maximized": false }
    property string language: getDefaults()["language"]
    property string audioOutputDevice: getDefaults()["audioOutputDevice"]
    property string audioInputDevice: getDefaults()["audioInputDevice"]
    property real micPassthroughEnabled: getDefaults()["micPassthroughEnabled"]

    onOutputVolumeChanged: audioSystem.outputDevice.volume = outputVolume
    onInputVolumeChanged: audioSystem.micPassthrough.outputDevice.volume = inputVolume
    onOutputOverdriveChanged: audioSystem.outputDevice.overdrive = outputOverdrive
    onInputOverdriveChanged: audioSystem.micPassthrough.outputDevice.overdrive = inputOverdrive
    onLanguageChanged: translator.currentLanguage = language
    onMicPassthroughEnabledChanged: audioSystem.micPassthrough.inputDevice.enabled = micPassthroughEnabled
    onAudioOutputDeviceChanged: audioSystem.outputDevice.device = audioSystem.getOutputDeviceById(audioOutputDevice)
    onAudioInputDeviceChanged: audioSystem.micPassthrough.inputDevice.device = audioSystem.getInputDeviceById(audioInputDevice)

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
