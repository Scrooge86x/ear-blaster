pragma Singleton

import QtCore

Settings {
    location: StandardPaths.writableLocation(StandardPaths.ConfigLocation) + "/config.ini"

    property string sounds: "[]"
    property real mainVolume: 1.0

    enum CloseBehavior {
        Quit,
        HideToTray,
        HideKeepTray
    }
    property real closeBehavior: AppSettings.CloseBehavior.HideToTray
}
