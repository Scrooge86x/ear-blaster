import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TextField {
    property string sequence
    property string globalSequence

    id: root
    readOnly: true
    placeholderText: qsTr("Choose hotkey...")
    Component.onCompleted: text = sequence

    background: Rectangle {
        anchors.fill: parent
        color: Universal.background
        border.width: parent.focus ? 1 : 0
        border.color: Universal.accent
        radius: 7
    }

    onEditingFinished: {
        if (!text || text.endsWith("+")) {
            text = sequence;
            return;
        }
        sequence = text;
    }

    Connections {
        enabled: root.focus
        target: globalKeyListener
        function onGlobalHotkeyPressed(hotkey) {
            globalSequence = hotkey;
        }
    }

    Keys.onPressed: (event) => {
        if (event.isAutoRepeat || !event.key) {
            return;
        }

        switch (event.key) {
        case Qt.Key_Backspace:
            text = sequence = "";
            return editingFinished();
        case Qt.Key_Escape:
            text = "";
            return editingFinished();
        case Qt.Key_Enter:
        case Qt.Key_Return:
            return;
        }

        text = globalSequence;
        event.accepted = true;
    }
}
