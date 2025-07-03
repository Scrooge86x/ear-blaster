import QtQuick
import QtQuick.Controls.Universal

Item {
    signal backClicked()

    Button {
        text: "Back"
        onClicked: backClicked()
    }
}
