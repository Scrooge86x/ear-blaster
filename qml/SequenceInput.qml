import QtQuick
import QtQuick.Controls.Universal

TextField {
    property string sequence

    id: root
    readOnly: true
    placeholderText: qsTr("Choose hotkey...")
    Component.onCompleted: text = sequence

    background: Rectangle {
        anchors.fill: parent
        color: AppSettings.backgroundColor
        border.width: parent.focus ? 1 : 0
        border.color: AppSettings.accentColor
        radius: 7
    }

    onEditingFinished: {
        if (!text || text.endsWith("+") && !text.endsWith("Num+")) {
            text = sequence;
            return;
        }
        sequence = text;
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

        text = globalKeyListener.currentSequence;
        event.accepted = true;
    }
}
