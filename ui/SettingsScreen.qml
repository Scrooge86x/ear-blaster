import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import QtQuick.Dialogs
import ui.settings
import ui.components

Item {
    id: root

    signal backClicked()

    Dialog {
        id: restoreDefaultsDialog
        modal: true
        anchors.centerIn: root
        title: qsTr("Are you sure?")
        standardButtons: Dialog.Yes | Dialog.No
        onAccepted: AppSettings.restoreDefaults()

        Label {
            text: qsTr("This will restore every setting to their default value.")
        }
    }
    ColumnLayout {
        Button {
            text: qsTr("Back")
            onClicked: root.backClicked()
        }

        Button {
            text: qsTr("Restore defaults")
            onClicked: restoreDefaultsDialog.open()
        }

        Label {
            text: qsTr("On close behavior:")
        }
        ComboBox  {
            model: [qsTr("Quit"), qsTr("Hide to tray"), qsTr("Hide (tray always visible)")]
            Layout.preferredWidth: 240

            currentIndex: AppSettings.closeBehavior
            delegate: ItemDelegate {
                width: parent.width
                text: modelData
                highlighted: parent.highlightedIndex === index
            }
            onActivated: (index) => AppSettings.closeBehavior = index
        }

        Label {
            text: qsTr("On second hotkey press:")
        }
        ComboBox  {
            model: [qsTr("Restart sound"), qsTr("Stop sound")]
            Layout.preferredWidth: 150

            currentIndex: AppSettings.secondPressBehavior
            delegate: ItemDelegate {
                width: parent.width
                text: modelData
                highlighted: parent.highlightedIndex === index
            }
            onActivated: (index) => AppSettings.secondPressBehavior = index
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
                border {
                    width: 1
                    color: AppSettings.foregroundColor
                }
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
            delegate: ItemDelegate {
                width: parent.width
                text: translator.getLanguageFullName(modelData)
                highlighted: parent.highlightedIndex === index
            }
            onActivated: (index) => AppSettings.language = model[index]
        }

        Label {
            text: qsTr("Input device:")
        }
        RowLayout {
            VolumeInput {
                text: qsTr("Volume:")
                sliderWidth: 100
                Layout.rightMargin: 15
                value: AppSettings.inputVolume
                onValueChanged: AppSettings.inputVolume = value
            }

            VolumeInput {
                text: qsTr("Overdrive:")
                sliderWidth: 100
                value: AppSettings.inputOverdrive
                onValueChanged: AppSettings.inputOverdrive = value
            }

            AudioDeviceSelect {
                Layout.preferredWidth: 250
                appSettingsPropName: "audioInputDevice"
                deviceType: AudioDeviceSelect.DeviceType.Input
            }
        }

        Label {
            text: qsTr("Monitor device:")
        }
        RowLayout {
            VolumeInput {
                text: qsTr("Volume:")
                sliderWidth: 100
                Layout.rightMargin: 15
                enabled: !AppSettings.audioMonitorMatchOutput
                value: AppSettings.monitorVolume
                onValueChanged: AppSettings.monitorVolume = value
            }

            VolumeInput {
                text: qsTr("Overdrive:")
                sliderWidth: 100
                enabled: !AppSettings.audioMonitorMatchOutput
                value: AppSettings.monitorOverdrive
                onValueChanged: AppSettings.monitorOverdrive = value
            }

            CheckBox {
                text: qsTr("Match main output")
                checked: AppSettings.audioMonitorMatchOutput
                onToggled: AppSettings.audioMonitorMatchOutput = checked
            }

            AudioDeviceSelect {
                Layout.preferredWidth: 250
                appSettingsPropName: "audioMonitorDevice"
                deviceType: AudioDeviceSelect.DeviceType.Output
            }
        }
    }
}
