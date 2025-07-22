import QtQuick
import QtQuick.Controls.Universal
import ui.settings

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
    displayText: root.currentText.length ? root.currentText : qsTr("No device selected.")

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
        const wasDeviceDisconnected = root.currentIndex === 0 && root.highlightedIndex === -1;
        if (wasDeviceDisconnected) {
            root.currentIndex = internal.getDeviceIndexByIdFn(AppSettings[root.appSettingsPropName]);
            AppSettings[root.appSettingsPropName] = root.valueAt(currentIndex);
        }
    }
    onActivated: (index) => AppSettings[root.appSettingsPropName] = root.valueAt(index)

    // Cannot use indexOfValue here
    Component.onCompleted: root.currentIndex = internal.getDeviceIndexByIdFn(AppSettings[root.appSettingsPropName])

    QtObject {
        id: internal
        property var getDeviceIndexByIdFn: switch (root.deviceType) {
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
