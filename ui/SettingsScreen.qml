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

        // Using ColumnLayout fixes: QML Dialog: Binding loop detected for property "implicitWidth"
        ColumnLayout {
            Label {
                text: qsTr("This will restore every setting to their default value.")
            }
        }
    }
    ColumnLayout {
        spacing: 15

        ColumnLayout {
            Button {
                icon.source: "qrc:/qt/qml/ui/resources/pictogrammers/back-arrow.svg"
                onClicked: root.backClicked()
            }
            Button {
                text: qsTr("Restore defaults")
                onClicked: restoreDefaultsDialog.open()
            }
        }

        ColumnLayout {
            Label {
                text: qsTr("On close behavior:")
            }
            ComboBox {
                id: closeBehaviorComboBox
                model: [QT_TR_NOOP("Quit"), QT_TR_NOOP("Hide to tray"), QT_TR_NOOP("Hide (tray always visible)")]
                Layout.preferredWidth: 240

                currentIndex: AppSettings.closeBehavior
                displayText: qsTr(model[currentIndex])
                delegate: ItemDelegate {
                    width: parent.width
                    text: qsTr(closeBehaviorComboBox.model[index])
                    highlighted: parent.highlightedIndex === index
                }
                onActivated: (index) => AppSettings.closeBehavior = index
            }
        }

        ColumnLayout {
            Label {
                text: qsTr("On second hotkey press:")
            }
            ComboBox {
                id: secondPressBehaviorComboBox
                model: [QT_TR_NOOP("Restart sound"), QT_TR_NOOP("Stop sound")]
                Layout.preferredWidth: 240

                currentIndex: AppSettings.secondPressBehavior
                displayText: qsTr(model[currentIndex])
                delegate: ItemDelegate {
                    width: parent.width
                    text: qsTr(secondPressBehaviorComboBox.model[index])
                    highlighted: parent.highlightedIndex === index
                }
                onActivated: (index) => AppSettings.secondPressBehavior = index
            }
        }

        ColumnLayout {
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
        }

        ColumnLayout {
            Label {
                text: qsTr("Save window geometry")
            }
            Switch {
                Layout.leftMargin: -5
                text: checked ? qsTr("On") : qsTr("Off")
                checked: AppSettings.windowGeometry
                onToggled: AppSettings.windowGeometry = checked ? "{}" : ""  // Anything other than "" will get filled in during shutdown
            }
        }

        ColumnLayout {
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
        }

        ColumnLayout {
            spacing: 0

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
        }

        ColumnLayout {
            spacing: 0

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

        ColumnLayout {
            Label {
                text: qsTr("Text to speech locale:")
            }
            ComboBox {
                id: ttsLocale
                model: audioSystem.tts.availableLocales()
                Layout.preferredWidth: 240

                displayText: model[currentIndex].nativeLanguageName
                delegate: ItemDelegate {
                    width: parent.width
                    text: ttsLocale.model[index].nativeLanguageName
                    highlighted: parent.highlightedIndex === index
                }
                onActivated: (index) => AppSettings.ttsLocale = model[index].name

                Component.onCompleted: {
                    let localeIndex = -1;
                    for (let i = 0; i < model.length; ++i) {
                        if (model[i].name === AppSettings.ttsLocale) {
                            localeIndex = i;
                            break;
                        }
                    }

                    if (localeIndex !== -1) {
                        ttsLocale.currentIndex = localeIndex
                    } else {
                        AppSettings.ttsLocale = model[0].name;
                        ttsLocale.currentIndex = 0;
                    }
                }
            }
        }
    }
}
