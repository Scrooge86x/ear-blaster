import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Rectangle {
    property ListModel listModel

    id: root
    clip: true
    color: Universal.background

    MouseArea {
        id: focusStealer
        anchors.fill: root
        enabled: false
        z: 0
        onClicked: forceActiveFocus()
        preventStealing: true
        propagateComposedEvents: true
    }

    property int uniqueIndex: 0
    property bool disablePlayback: false

    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false
            anchors {
                left: parent?.left
                right: parent?.right
            }
            height: content.height

            drag.target: held ? content : undefined
            drag.axis: Drag.YAxis
            cursorShape: Qt.ClosedHandCursor

            onPressed: {
                focusStealer.z = 1
                focusStealer.cursorShape = Qt.ClosedHandCursor
                held = true
                z = -1
            }

            function releasedHandler() {
                focusStealer.z = 0
                focusStealer.cursorShape = Qt.ArrowCursor
                held = false
                z = 0
            }
            onReleased: releasedHandler()
            onCanceled: releasedHandler()

            SoundElement {
                id: content
                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                states: State {
                    when: dragArea.held

                    ParentChange {
                        target: content
                        parent: listView
                    }
                }

                disablePlayback: root.disablePlayback
                path: model.path
                Component.onCompleted: {
                    name = model.name // Avoids a binding loop in onNameChanged
                    sequence = model.sequence
                    initialIndex = uniqueIndex++
                }
                onDeleteRequested: listModel.remove(index, 1)
                onNameChanged: listModel.setProperty(index, "name", name)
                onSequenceChanged: listModel.setProperty(index, "sequence", sequence)
                onFocusChanged: (hasFocus) => {
                    focusStealer.z = hasFocus ? 1 : 0;
                    focusStealer.enabled = hasFocus;
                    root.disablePlayback = hasFocus;
                }
            }

            DropArea {
                anchors.fill: parent
                onEntered: (drag) => listView.model.items.move(
                    drag.source.DelegateModel.itemsIndex,
                    dragArea.DelegateModel.itemsIndex
                )
            }
        }
    }

    ColumnLayout {
        anchors.fill: root

        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            ScrollBar.vertical.width: 9
            ScrollBar.vertical.interactive: true

            padding: 0
            rightPadding: ScrollBar.vertical.width + 7

            ListView {
                id: listView
                spacing: 3
                cacheBuffer: 128

                model: DelegateModel {
                    model: listModel
                    delegate: dragDelegate
                }

                displaced: Transition {
                    NumberAnimation {
                        properties: "y"
                        duration: 200
                    }
                }
            }
        }
    }
}
