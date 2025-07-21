import QtQuick
import QtQuick.Controls.Universal

ComboBox {
    id: root

    enum DeviceType {
        Input,
        Output
    }
    required property int deviceType
    required property string appSettingsPropName

    valueRole: "id"
    textRole: "description"
    displayText: currentText.length ? currentText : qsTr("No device selected.")

    model: switch (deviceType) {
        case AudioDeviceSelect.DeviceType.Input:
            audioSystem.audioInputs;
            break;
        case AudioDeviceSelect.DeviceType.Output:
            audioSystem.audioOutputs;
            break;
        default:
            break;
    }
    delegate: ItemDelegate {
        width: ListView.view.width
        text: modelData.description.length ? modelData.description : qsTr("No device selected.")
        font.weight: root.currentIndex === index ? Font.DemiBold : Font.Normal
        highlighted: root.highlightedIndex === index
        hoverEnabled: root.hoverEnabled
    }

    onCurrentIndexChanged: {
        const wasDeviceDisconnected = currentIndex === 0 && highlightedIndex === -1;
        if (wasDeviceDisconnected) {
            currentIndex = internal.getDeviceIndexByIdFn(AppSettings[appSettingsPropName]);
            AppSettings[appSettingsPropName] = valueAt(currentIndex);
        }
    }
    onActivated: (index) => AppSettings[appSettingsPropName] = valueAt(index)

    // Cannot use indexOfValue here
    Component.onCompleted: currentIndex = internal.getDeviceIndexByIdFn(AppSettings[appSettingsPropName])

    QtObject {
        id: internal
        property var getDeviceIndexByIdFn: switch (deviceType) {
            case AudioDeviceSelect.DeviceType.Input:
                (id) => audioSystem.getInputDeviceIndexById(id);
                break;
            case AudioDeviceSelect.DeviceType.Output:
                (id) => audioSystem.getOutputDeviceIndexById(id);
                break;
            default:
                break;
        }
    }
}
