import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

RowLayout {
    id: root

    property alias text: label.text
    property alias value: slider.value
    property double sliderWidth: 75

    Label {
        id: label
        color: AppSettings.foregroundColor
    }

    Slider {
        id: slider
        from: 0.0
        to: 1.0
        stepSize: 0.01
        Layout.preferredWidth: root.sliderWidth
    }

    Label {
        Layout.preferredWidth: 35
        text: `${Math.round(slider.value * 100)}%`
        color: AppSettings.foregroundColor
        font.pixelSize: 13
    }
}
