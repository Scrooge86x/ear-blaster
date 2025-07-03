import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import QtQuick.Dialogs

Item {
    signal backClicked()

    ColumnLayout {
        Button {
            text: "Back"
            onClicked: backClicked()
        }

        Label {
            text: "On close behavior:"
        }
        ComboBox  {
            model: ["Quit", "Hide to tray", "Hide (tray always visible)"]
            Layout.preferredWidth: 240

            currentIndex: AppSettings.closeBehavior
            onActivated: (index) => AppSettings.closeBehavior = index
            delegate: ItemDelegate {
                width: parent.width
                text: modelData
                highlighted: parent.highlightedIndex === index
            }
        }

        Label {
            text: "On second hotkey press:"
        }
        ComboBox  {
            model: ["Restart sound", "Stop sound"]
            Layout.preferredWidth: 150

            currentIndex: AppSettings.secondPressBehavior
            onActivated: (index) => AppSettings.secondPressBehavior = index
            delegate: ItemDelegate {
                width: parent.width
                text: modelData
                highlighted: parent.highlightedIndex === index
            }
        }

        Label {
            text: "App colors:"
        }
        RowLayout {
            Rectangle {
                Layout.preferredWidth: 10
                Layout.fillHeight: true
                color: AppSettings.foregroundColor
            }
            Button {
                text: "foreground"
                onClicked: foregroundColorDialog.open()

                ColorDialog {
                    id: foregroundColorDialog
                    selectedColor: AppSettings.foregroundColor
                    onAccepted: AppSettings.foregroundColor = selectedColor
                }
            }

            Rectangle {
                Layout.preferredWidth: 10
                Layout.fillHeight: true
                color: AppSettings.backgroundColor
                border.width: 1
                border.color: AppSettings.foregroundColor
            }
            Button {
                text: "background"
                onClicked: backgroundColorDialog.open()

                ColorDialog {
                    id: backgroundColorDialog
                    selectedColor: AppSettings.backgroundColor
                    onAccepted: AppSettings.backgroundColor = selectedColor
                }
            }

            Rectangle {
                Layout.preferredWidth: 10
                Layout.fillHeight: true
                color: AppSettings.accentColor
            }
            Button {
                text: "accent"
                onClicked: accentColorDialog.open()

                ColorDialog {
                    id: accentColorDialog
                    selectedColor: AppSettings.accentColor
                    onAccepted: AppSettings.accentColor = selectedColor
                }
            }
        }
    }
}
