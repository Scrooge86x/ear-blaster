import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal
import ui.settings

Rectangle {
    id: root

    property ListModel listModel
    property int uniqueIndex: 0
    property bool disablePlayback: false
    property bool stealingFocus: false

    function removeSound(index) {
        soundRemovedPopup.close();

        const lastSong = { index };
        Object.assign(lastSong, root.listModel.get(index));
        soundRemovedPopup.lastRemovedSong = lastSong;
        root.listModel.remove(index, 1);

        soundRemovedPopup.open();
    }

    clip: true
    color: AppSettings.backgroundColor

    onStealingFocusChanged: {
        if (root.stealingFocus) {
            focusStealer.z = 1
            focusStealer.cursorShape = Qt.ClosedHandCursor
        } else {
            focusStealer.z = 0
            focusStealer.cursorShape = Qt.ArrowCursor
        }
    }

    Popup {
        id: soundRemovedPopup

        property var lastRemovedSong

        closePolicy: Popup.NoAutoClose
        y: root.height - soundRemovedPopup.height
        onAboutToShow: soundRemovedAnimation.start();
        onClosed: {
            soundRemovedPopup.lastRemovedSong = null;
            soundRemovedAnimation.stop();
        }

        ColumnLayout {
            RowLayout {
                Label {
                    text: qsTr('Sound "%1" removed.').arg(soundRemovedPopup.lastRemovedSong?.name)
                }

                Item {
                    Layout.fillWidth: true
                }

                RoundButton {
                    icon.source: "qrc:/qt/qml/ui/resources/pictogrammers/undo-arrow.svg"
                    radius: 7
                    leftPadding: 15
                    rightPadding: 15

                    onClicked: {
                        const lastSong = soundRemovedPopup.lastRemovedSong;
                        if (!lastSong) {
                            return;
                        }

                        const index = lastSong.index;
                        delete lastSong.index;

                        root.listModel.insert(index, lastSong);
                        soundRemovedPopup.close();
                    }
                }
            }
            ProgressBar {
                Layout.fillWidth: true
                value: 1

                NumberAnimation on value {
                    id: soundRemovedAnimation
                    running: false
                    from: 1
                    to: 0
                    duration: 4000

                    onFinished: soundRemovedPopup.close()
                }
            }
        }
    }

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
                root.stealingFocus = false;
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
                root.stealingFocus = true;
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
                onDeleteRequested: root.removeSound(index)
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
