pragma Singleton

import QtCore

Settings {
    location: StandardPaths.writableLocation(StandardPaths.ConfigLocation) + "/config.ini"

    property string sounds: "[]"
    property real mainVolume: 1.0
    property string foregroundColor: "#ddd"
    property string backgroundColor: "#0f0f0f"
    property string accentColor: "#fa6800"

    enum CloseBehavior {
        Quit,
        HideToTray,
        HideKeepTray
    }
    property real closeBehavior: AppSettings.CloseBehavior.HideKeepTray

    enum SecondPressBehavior {
        StartOver,
        StopSound
    }
    property real secondPressBehavior: AppSettings.SecondPressBehavior.StopSound
}
