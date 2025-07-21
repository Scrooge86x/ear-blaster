import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Rectangle {
    id: root

    property ListModel listModel
    property int uniqueIndex: 0
    property bool disablePlayback: false

    clip: true
    color: AppSettings.backgroundColor

    MouseArea {
        id: focusStealer
        anchors.fill: root
        enabled: false
        z: 0
        preventStealing: true
        propagateComposedEvents: true
        onClicked: forceActiveFocus()
    }

    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false

            function releasedHandler() {
                focusStealer.z = 0
                focusStealer.cursorShape = Qt.ArrowCursor
                held = false
                z = 0
            }

            cursorShape: Qt.ClosedHandCursor
            height: content.height
            anchors {
                left: parent?.left
                right: parent?.right
            }
            drag {
                target: held ? content : undefined
                axis: Drag.YAxis
            }

            onPressed: {
                focusStealer.z = 1
                focusStealer.cursorShape = Qt.ClosedHandCursor
                held = true
                z = -1
            }

            onReleased: releasedHandler()
            onCanceled: releasedHandler()

            SoundElement {
                id: content

                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2
                Drag.keys: ["sound-list"]

                disablePlayback: root.disablePlayback
                path: model.path
                states: State {
                    when: dragArea.held

                    ParentChange {
                        target: content
                        parent: listView
                    }
                }

                Component.onCompleted: {
                    name = model.name // Avoids a binding loop in onNameChanged
                    sequence = model.sequence
                    initialIndex = root.uniqueIndex++
                }
                onDeleteRequested: root.listModel.remove(index, 1)
                onNameChanged: root.listModel.setProperty(index, "name", name)
                onSequenceChanged: root.listModel.setProperty(index, "sequence", sequence)
                onFocusChanged: (hasFocus) => {
                    focusStealer.z = hasFocus ? 1 : 0;
                    focusStealer.enabled = hasFocus;
                    root.disablePlayback = hasFocus;
                }
            }

            DropArea {
                keys: ["sound-list"]
                anchors.fill: parent
                onEntered: (drag) => root.listModel.move(
                    drag.source.DelegateModel.itemsIndex,
                    dragArea.DelegateModel.itemsIndex,
                    1
                );
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
                    model: root.listModel
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
