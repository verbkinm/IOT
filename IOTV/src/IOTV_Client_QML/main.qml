import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.15

import "Home" as HomePageModule
import "Client" as ClientPageModule
import "Events" as EventsPageModule

ApplicationWindow {
    id: window
    width: 360
    height: 640
    visible: true
    title: qsTr("IOTV Client")

    minimumWidth: 360
    minimumHeight: 520

    //! [orientation]
    readonly property bool inPortrait: window.width < window.height
    //! [orientation]

    property alias appStack: stackView

    header: ToolBar {
        height: 50
        id: overlayHeader

        ToolButton {
            id: pressBack
            icon {
                color: "transparent"
                source: "qrc:/img/back.png"
            }

            anchors{
                verticalCenter: parent.verticalCenter
                leftMargin: 20
                left: parent.left
            }
            onClicked: {
                window.close()
            }
        }

        Label {
            width: parent.width
            height: parent.height
            text: stackView.currentItem.title
            elide: Text.ElideRight
            font.pixelSize: 18
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            leftPadding: pressBack.width + pressBack.anchors.leftMargin
            rightPadding: pressBack.width + pressBack.anchors.leftMargin
        }

        ToolButton {
            id: menu
            icon {
                color: "transparent"
                source: "qrc:/img/menu.png"
            }

            anchors{
                verticalCenter: parent.verticalCenter
                rightMargin: 20
                right: parent.right
            }
            onClicked: {
                drawer.visible = !drawer.visible
            }
        }
    }

    MainMenu {
        id: drawer
        y: overlayHeader.height
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: homePage

        HomePageModule.Home {
            id: homePage
            objectName: "Home"
        }

        EventsPageModule.Events {
            id: eventsPage
            objectName: "Events"
            visible: false
        }

        ClientPageModule.Client {
            id: clientPage
            objectName: "Client"
            visible: false
        }

        onCurrentItemChanged: {
            console.log("stackView current item: ", stackView.currentItem.objectName)
            if (appStack.currentItem == homePage)
                loaderDevice.source = ""
        }
    }

    footer: ToolBar {
        height: overlayHeader.height
        id: overlayFooter

        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon {
                    color: "transparent"
                    source: "qrc:/img/home.png"
                }
                onClicked: {
                    appStack.pop(homePage)
                }
                Layout.alignment: Qt.AlignCenter
            }

            ToolButton {
                icon {
                    color: "transparent"
                    source: "qrc:/img/calendar_white.png"
                }
                onClicked: {
                    if (appStack.currentItem == clientPage)
                        appStack.pop()
                    appStack.push(eventsPage)
                }
                Layout.alignment: Qt.AlignCenter
            }

            ToolButton {
                icon {
                    color: "transparent"
                    source: "qrc:/img/settings_white.png"
                }
                onClicked: {
                    if (appStack.currentItem == eventsPage)
                        appStack.pop()
                    appStack.push(clientPage)

                    console.log(appStack.depth)

                }
                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    DialogShared {
        id: dialogExit
        parent: stackView
        standardButtons: Dialog.Yes | Dialog.No
        text: "Вы действительно хотите выйти?"
        visible: false

        onAccepted: {
            Qt.exit(0)
        }
    }

    // Загружаются DialogShared и Notification со всей программы
    Loader {
        id: loaderMainItem
        source: ""
    }

    // Загружаются устройства
    Loader {
        property string title
        id: loaderDevice
        source: ""
    }

    onClosing: {
        close.accepted = false
        if (appStack.currentItem == homePage)
            dialogExit.open()
        else
            appStack.pop()
    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:5}D{i:7}D{i:8}D{i:9}D{i:6}D{i:15}D{i:16}
D{i:17}
}
##^##*/
