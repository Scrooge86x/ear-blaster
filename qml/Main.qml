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
    width: 900
    minimumWidth: 600
    height: 650
    minimumHeight: 400
    title: "Ear Blaster"
    color: AppSettings.backgroundColor

    topPadding: 7
    rightPadding: 7
    bottomPadding: 7
    leftPadding: 7

    onClosing: (close) => {
        if (AppSettings.closeBehavior == AppSettings.CloseBehavior.Quit) {
            return;
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
                onTriggered: Qt.exit(0)
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
