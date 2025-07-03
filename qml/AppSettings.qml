pragma Singleton

import QtCore

Settings {
    location: StandardPaths.writableLocation(StandardPaths.ConfigLocation) + "/config.ini"

    // If this was an object instead of a function it would get saved
    // in the config file
    function getDefault(key) {
        return {
            sounds: "[]",
            mainVolume: 1.0,
            foregroundColor: "#ddd",
            backgroundColor: "#0f0f0f",
            accentColor: "#fa6800",
            closeBehavior: AppSettings.CloseBehavior.HideKeepTray,
            secondPressBehavior: AppSettings.SecondPressBehavior.StopSound,
        }[key];
    }

    property string sounds: getDefault("sounds")
    property real mainVolume: getDefault("mainVolume")
    property string foregroundColor: getDefault("foregroundColor")
    property string backgroundColor: getDefault("backgroundColor")
    property string accentColor: getDefault("accentColor")

    enum CloseBehavior {
        Quit,
        HideToTray,
        HideKeepTray
    }
    property real closeBehavior: getDefault("closeBehavior")

    enum SecondPressBehavior {
        StartOver,
        StopSound
    }
    property real secondPressBehavior: getDefault("secondPressBehavior")

    function restoreDefaults() {
        sounds              = getDefault("sounds")
        mainVolume          = getDefault("mainVolume")
        foregroundColor     = getDefault("foregroundColor")
        backgroundColor     = getDefault("backgroundColor")
        accentColor         = getDefault("accentColor")
        closeBehavior       = getDefault("closeBehavior")
        secondPressBehavior = getDefault("secondPressBehavior")
    }
}
