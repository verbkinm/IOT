import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

ApplicationWindow {

    id: window
    width: 360
    height: 520
    visible: true
    title: qsTr("IOTV Client")

    //! [orientation]
    readonly property bool inPortrait: window.width < window.height
    //! [orientation]



        Menu {
            id: menu

            MenuItem {
                text: "New..."
                onTriggered: document.reset()
            }
            MenuItem {
                text: "Open..."
                onTriggered: openDialog.open()
            }
            MenuItem {
                text: "Save"
                onTriggered: saveDialog.open()
            }
        }



    header: ToolBar {
        height: 50
        id: overlayHeader
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr("🏠")
                onClicked: {
                    drawer.visible = false
                    swipeView.setCurrentIndex(p1)
                }
            }
            Label {
                text: swipeView.itemAt(swipeView.currentIndex).title
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton {
                text: qsTr("⋮")
                onClicked: {
                    drawer.visible = !drawer.visible
                }
            }
        }
    }

    Drawer {
        id: drawer

        y: overlayHeader.height
        width: window.width / 2
        height: window.height - overlayHeader.height

        modal: inPortrait
        interactive: inPortrait
        position: inPortrait ? 0 : 1
        visible: !inPortrait


        ListModel {
            id: listModel

            ListElement {
                name: "Home"
            }
            ListElement {
                name: "Server"
            }
        }

        Component {
            id: delegate
            Item {
                width: parent.width; height: 30
                Rectangle {
                    anchors.fill: parent
                    color: "transparent"

                    MouseArea{
                        anchors.fill: parent
                        anchors.centerIn: parent
                        Text {
                            anchors.centerIn: parent
                            text: name;
                            font.pointSize: 12
                        }
                        onClicked: {
                            swipeView.setCurrentIndex(index)
                            drawer.visible = 0
                        }
                    }
                }
            }
        }

        ListView {
            id: listView
            anchors.fill: parent
            model: listModel
            delegate: delegate
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent

        Home {
            id: p1
            status: client.status
            countDevice: client.countDevice
            countDeviceOnline: client.countDeviceOnline
        }

        Client {
            id: client
        }
    }

    PageIndicator {
         id: indicator

         count: swipeView.count
         currentIndex: swipeView.currentIndex

         anchors.bottom: swipeView.bottom
         anchors.horizontalCenter: parent.horizontalCenter
    }
}

