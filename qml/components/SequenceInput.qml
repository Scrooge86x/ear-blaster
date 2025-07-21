import QtQuick
import QtQuick.Controls.Universal

TextField {
    id: root

    property string sequence

    readOnly: true
    placeholderText: qsTr("Choose hotkey...")
    Component.onCompleted: root.text = root.sequence

    background: Rectangle {
        anchors.fill: root
        color: AppSettings.backgroundColor
        border.width: root.focus ? 1 : 0
        border.color: AppSettings.accentColor
        radius: 7
    }

    onEditingFinished: {
        if (!root.text || root.text.endsWith("+") && !root.text.endsWith("Num+")) {
            root.text = root.sequence;
            return;
        }
        root.sequence = root.text;
    }

    Keys.onPressed: (event) => {
        if (event.isAutoRepeat || !event.key) {
            return;
        }

        switch (event.key) {
        case Qt.Key_Backspace:
            root.text = root.sequence = "";
            return root.editingFinished();
        case Qt.Key_Escape:
            root.text = "";
            return root.editingFinished();
        case Qt.Key_Enter:
        case Qt.Key_Return:
            return;
        }

        root.text = globalKeyListener.currentSequence;
        event.accepted = true;
    }
}
