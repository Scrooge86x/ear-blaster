import QtQuick

Item {
    Component.onCompleted: {
        // Update the values with defaults (check AppSettings.qml)
        if (AppSettings.ttsLocale === "") {
            AppSettings.ttsLocale = " ";
            AppSettings.ttsVoice = " ";
        }
        if (AppSettings.ttsVoice === "") {
            AppSettings.ttsVoice = " ";
        }
    }

    function invalidateDisconnectedDevices(devicesToCheck, deviceList) {
        const isInvalidDevice = {};
        devicesToCheck.forEach((deviceKey) => isInvalidDevice[deviceKey] = true);

        for (const device of deviceList) {
            const deviceId = device.id.toString();
            for (const deviceKey in isInvalidDevice) {
                if (deviceId === AppSettings[deviceKey].toString()) {
                    isInvalidDevice[deviceKey] = false;
                }
            }
        }

        for (const deviceKey in isInvalidDevice) {
            if (isInvalidDevice[deviceKey]) {
                AppSettings[deviceKey] = "";
            }
        }
    }

    Connections {
        target: audioSystem

        function onAudioOutputsChanged() {
            invalidateDisconnectedDevices(
                ["audioOutputDevice", "audioMonitorDevice"],
                audioSystem.audioOutputs
            );
        }

        function onAudioInputsChanged() {
            invalidateDisconnectedDevices(
                ["audioInputDevice"],
                audioSystem.audioInputs
            );
        }
    }
}
