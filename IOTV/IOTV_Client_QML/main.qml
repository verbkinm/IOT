import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3
import Qt.labs.platform 1.1

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
                    drawer.visible = false
                    stackView.pop(homePage)
                }
            }
            Label {
                text: stackView.currentItem.title
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
                            if (index === 0)
                                stackView.pop(homePage)
                            else if (index === 1 && stackView.currentItem != clientPage)
                                stackView.push(clientPage)

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

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: homePage

        Home {
            id: homePage
        }

        Client {
            id: clientPage
            visible: false
        }
    }

//    Popup {
//        id: popup
//        anchors.centerIn: parent
//        //            x: 100
//        //            y: 100
////        width: 200
////        height: 300
//        modal: true
//        focus: true
//        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent | Popup.CloseOnPressOutsideParent
//        opacity: 0.0

////        MouseArea {
////            width: 200
////            height: 200
////            onClicked: popup.close()
////        }

//        Text { text: "popup"}
//        BusyIndicator {
//            id: indicator
//            antialiasing: true
//            anchors.centerIn: parent
//        }
//    }
//    Component.onCompleted: {popup.open()}
}

