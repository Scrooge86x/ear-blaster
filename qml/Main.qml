import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal
import QtQuick.Dialogs
import QtMultimedia
import Qt.labs.platform

ApplicationWindow {
    id: root

    Universal.theme: Universal.Dark
    Universal.foreground: AppSettings.foregroundColor
    Universal.background: AppSettings.backgroundColor
    Universal.accent: AppSettings.accentColor

    visible: true
    minimumWidth: 975
    minimumHeight: 700
    title: "Ear Blaster"
    color: AppSettings.backgroundColor

    topPadding: 7
    rightPadding: 7
    bottomPadding: 7
    leftPadding: 7

    function exitApp() {
        if (AppSettings.windowGeometry !== "") {
            AppSettings.windowGeometry = JSON.stringify({
                x: root.x,
                y: root.y,
                width: root.width,
                height: root.height,
                maximized: root.visibility === Window.Maximized,
            });
        }
        Qt.exit(0);
    }

    Component.onCompleted: {
        if (AppSettings.windowGeometry === "") {
            return;
        }

        try {
            const windowGeometry = JSON.parse(AppSettings.windowGeometry);
            const expectedKeys = ["x", "y", "width", "height", "maximized"];
            if (Object.keys(windowGeometry).length !== expectedKeys.length
                    || !expectedKeys.every(key => key in windowGeometry)) {
                return console.error("Invalid window geometry configuration detected: ", AppSettings.windowGeometry);
            }

            if (windowGeometry.maximized) {
                root.visibility = Window.Maximized;
            } else {
                root.x      = windowGeometry.x;
                root.y      = windowGeometry.y;
                root.width  = Math.max(windowGeometry.width,  root.minimumWidth);
                root.height = Math.max(windowGeometry.height, root.minimumHeight);
            }
        } catch (error) {
            console.error("Error while reading windowData: ", error);
        }
    }

    onClosing: (close) => {
        if (AppSettings.closeBehavior == AppSettings.CloseBehavior.Quit) {
            return exitApp();
        }

        root.hide();
        trayIcon.safeShow();
        close.accepted = false;
    }

    SystemTrayIcon {
        id: trayIcon
        icon.source: "qrc:/qt/qml/ear-blaster/resources/ear-blaster.ico"
        tooltip: "Ear Blaster"
        Component.onCompleted: {
            if (AppSettings.closeBehavior === AppSettings.CloseBehavior.HideKeepTray) {
                trayIcon.safeShow()
            }
        }
        menu: Menu {
            id: trayMenu

            MenuItem {
                id: trayMenuShow
                text: qsTr("Show")
                onTriggered: {
                    if (AppSettings.closeBehavior == AppSettings.CloseBehavior.HideToTray) {
                        trayIcon.safeHide();
                    }
                    root.show();
                }
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Exit")
                onTriggered: exitApp()
            }
        }

        // safeShow and safeHide functions are a workaround for the fact
        // that trayIcon.hide() causes the menu and icons to get destroyed
        function safeShow() {
            if (!trayIcon.visible) {
                trayIcon.show();
                trayIcon.menu = trayMenu;
                trayMenuShow.icon.source = "qrc:/qt/qml/ear-blaster/resources/ear-blaster.ico";
            }
        }

        function safeHide() {
            if (trayIcon.visible) {
                trayMenuShow.icon.source = undefined;
                trayIcon.menu = null;
                trayIcon.hide();
            }
        }
    }
    Connections {
        target: AppSettings
        function onCloseBehaviorChanged() {
            switch (AppSettings.closeBehavior) {
            case AppSettings.CloseBehavior.Quit:         trayIcon.safeHide(); break;
            case AppSettings.CloseBehavior.HideToTray:   trayIcon.safeHide(); break;
            case AppSettings.CloseBehavior.HideKeepTray: trayIcon.safeShow(); break;
            default:
                console.error("Invalid close behavior specified:", closeBehavior);
                break;
            }
        }
    }

    Component {
        id: mainScreen
        MainScreen {
            onSettingsClicked: stackView.push(settingsScreen)
        }
    }

    Component {
        id: settingsScreen
        SettingsScreen {
            onBackClicked: stackView.pop()
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mainScreen
    }
}
