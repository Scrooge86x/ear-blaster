import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal
import QtQuick.Dialogs
import QtMultimedia
import Qt.labs.platform

ApplicationWindow {
    id: root

    Universal.theme: Universal.Dark
    Universal.foreground: "#ddd"
    Universal.background: "#0f0f0f"
    Universal.accent: Universal.Orange

    visible: true
    width: 900
    minimumWidth: 600
    height: 650
    minimumHeight: 400
    title: "Ear Blaster"
    color: Universal.background

    topPadding: 7
    rightPadding: 7
    bottomPadding: 7
    leftPadding: 7

    onClosing: (close) => {
        root.hide();
        trayIcon.show();
        trayIcon.menu = trayMenu;
        close.accepted = false;
    }

    SystemTrayIcon {
        id: trayIcon
        icon.source: "qrc:/qt/qml/ear-blaster/resources/ear-blaster.ico"
        tooltip: "Ear Blaster"
        menu: Menu {
            id: trayMenu

            MenuItem {
                text: qsTr("Show")
                onTriggered: {
                    trayIcon.menu = null; // This is a workaround for the fact that trayIcon.hide() causes the menu to get destroyed
                    trayIcon.hide();
                    root.show();
                }
            }

            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.exit(0)
            }
        }
    }

    Component {
        id: mainScreen
        MainScreen {}
    }

    Loader {
        anchors.fill: parent
        sourceComponent: mainScreen
    }
}
