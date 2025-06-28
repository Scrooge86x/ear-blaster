import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

Rectangle {
    property ListModel listModel

    id: root
    clip: true
    color: Universal.background

    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false
            anchors {
                left: parent.left
                right: parent.right
            }
            height: content.height
            z: 0
            cursorShape: Qt.ClosedHandCursor

            drag.target: held ? content : undefined
            drag.axis: Drag.YAxis

            onPressed: {
                held = true
                z = -1
            }
            onReleased: {
                held = false
                z = 0
            }

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

                path: model.path
                sequence: model.sequence
                Component.onCompleted: name = model.name // Avoids a binding loop in onNameChanged
                onPlayRequested: soundPlayer.play(model.path)
                onStopRequested: soundPlayer.stop()
                onDeleteRequested: listModel.remove(index, 1)
                onNameChanged: listModel.setProperty(index, "name", name)
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
            ScrollBar.vertical.width: 12
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
