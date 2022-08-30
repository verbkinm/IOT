import QtQuick
import QtQuick.Controls
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

    header: ToolBar {
        height: 50
        id: overlayHeader
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr("🏠")
                onClicked: {
                    drawer.close()
                    swipeView.setCurrentIndex(p1)
                }
            }
            Label {
                text: swipeView.itemAt(swipeView.currentIndex).name
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton {
                text: qsTr("⋮")
                onClicked: {
                    if (drawer.opened)
                        drawer.close()
                    else
                        drawer.open()
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
                    //                    border.color: "red"
                    //                    border.width: 1

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

            //            headerPositioning: ListView.OverlayHeader
            //            header: Pane {
            //                id: header
            //                z: 2
            //                width: parent.width

            //                contentHeight: logo.height

            //                Image {
            //                    id: logo
            //                    width: parent.width / 2
            //                    anchors.centerIn: parent
            //                    source: "images/qt-logo.png"
            //                    fillMode: implicitWidth > width ? Image.PreserveAspectFit : Image.Pad
            //                }

            //                MenuSeparator {
            //                    parent: header
            //                    width: parent.width
            //                    anchors.verticalCenter: parent.bottom
            //                    visible: !listView.atYBeginning
            //                }
            //            }
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        //        currentIndex: tabBar.currentIndex

        Page {
            width: 600
            height: 400
            id: p1

            property string name: "Home"

            Flickable
            {
                anchors.topMargin: 15
                anchors.fill: parent
                contentHeight: childrenRect.height
                Label {
                    id: label1
                    text: "Home"
                    font.pixelSize: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.top: parent.top
                }
            }
        }

        Server_Page{
            id: server
            property string name: "Server"
        }

        Page {
            width: 600
            height: 400
            id: p3

            property string name: "Page 2"

            Flickable
            {
                anchors.topMargin: 15
                anchors.fill: parent
                contentHeight: childrenRect.height
                Label {
                    text: "label2: "
                    font.pixelSize: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.top: parent.top
                }

            }
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

