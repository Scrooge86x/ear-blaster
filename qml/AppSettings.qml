pragma Singleton

import QtCore

Settings {
    id: appSettings
    location: StandardPaths.writableLocation(StandardPaths.ConfigLocation) + "/config.ini"

    // If this was an object instead of a function it would get saved
    // in the config file
    function getDefaults() {
        return {
            mainVolume: 1.0,
            foregroundColor: "#ddd",
            backgroundColor: "#0f0f0f",
            accentColor: "#fa6800",
            closeBehavior: AppSettings.CloseBehavior.HideKeepTray,
            secondPressBehavior: AppSettings.SecondPressBehavior.StopSound,
        };
    }

    property string sounds: "[]"
    property real mainVolume: getDefaults()["mainVolume"]
    property string foregroundColor: getDefaults()["foregroundColor"]
    property string backgroundColor: getDefaults()["backgroundColor"]
    property string accentColor: getDefaults()["accentColor"]

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
