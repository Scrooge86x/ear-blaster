import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts

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
    }
}
