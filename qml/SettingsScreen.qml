import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import QtQuick.Dialogs

Item {
    signal backClicked()

    ColumnLayout {
        Button {
            text: qsTr("Back")
            onClicked: backClicked()
        }

        Button {
            text: qsTr("Restore defaults")
            onClicked: AppSettings.restoreDefaults()
        }

        Label {
            text: qsTr("On close behavior:")
        }
        ComboBox  {
            model: [qsTr("Quit"), qsTr("Hide to tray"), qsTr("Hide (tray always visible)")]
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
            text: qsTr("On second hotkey press:")
        }
        ComboBox  {
            model: [qsTr("Restart sound"), qsTr("Stop sound")]
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
            text: qsTr("App colors:")
        }
        RowLayout {
            Rectangle {
                Layout.preferredWidth: 10
                Layout.fillHeight: true
                color: AppSettings.foregroundColor
            }
            Button {
                text: qsTr("Foreground")
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
                text: qsTr("Background")
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
                text: qsTr("Accent")
                onClicked: accentColorDialog.open()

                ColorDialog {
                    id: accentColorDialog
                    selectedColor: AppSettings.accentColor
                    onAccepted: AppSettings.accentColor = selectedColor
                }
            }
        }

        CheckBox {
            text: qsTr("Save window geometry")
            checked: AppSettings.windowGeometry
            onToggled: AppSettings.windowGeometry = checked ? "{}" : ""  // Anything other than "" will get filled in during shutdown
        }

        Label {
            text: qsTr("Language:")
        }
        ComboBox  {
            model: translator.availableLanguages
            Layout.preferredWidth: 100

            currentIndex: model.indexOf(AppSettings.language)
            displayText: translator.getLanguageFullName(AppSettings.language)
            onActivated: (index) => AppSettings.language = model[index];
            delegate: ItemDelegate {
                width: parent.width
                text: translator.getLanguageFullName(modelData)
                highlighted: parent.highlightedIndex === index
            }
        }

        Label {
            text: qsTr("Input device:")
        }
        RowLayout {
            VolumeInput {
                text: qsTr("Volume:")
                value: AppSettings.inputVolume
                onValueChanged: AppSettings.inputVolume = value
                sliderWidth: 100
                Layout.rightMargin: 15
            }

            VolumeInput {
                text: qsTr("Overdrive:")
                value: AppSettings.inputOverdrive
                onValueChanged: AppSettings.inputOverdrive = value
                sliderWidth: 100
            }

            AudioDeviceSelect {
                Layout.preferredWidth: 250
                appSettingsPropName: "audioInputDevice"
                deviceType: AudioDeviceSelect.DeviceType.Input
            }
        }
    }
}
