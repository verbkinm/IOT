import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

import "Home" as HomePageModule
import "Client" as ClientPageModule

ApplicationWindow {
    id: window
    width: 360
    height: 520
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
                    drawer.visible = false
                    stackView.pop(homePage)
                }
                Layout.alignment: Qt.AlignCenter
            }
            ToolButton {
                icon {
                    color: "transparent"
                    source: "qrc:/img/menu.png"
                }
                onClicked: {
                    drawer.visible = !drawer.visible
                }
                Layout.alignment: Qt.AlignCenter
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
                name: "Главная"
            }
            ListElement {
                name: "Настройки"
            }
            ListElement {
                name: "О программе"
            }
            ListElement {
                name: "Выход"
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
                            {
                                stackView.push(clientPage)
                            }
                            else if (index === 2)
                            {
                                loaderDialog.setSource("DialogShared.qml",
                                                       {parent: stackView,
                                                           visible: true,
                                                           standardButtons: Dialog.Ok,
                                                           title: "О программе",
                                                           text: "Клиент IOTV " + Qt.application.version})
                            }

                            else if (index === 3)
                            {
//                                dialogExit.open()
                            }

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

        HomePageModule.Home {
            id: homePage
            objectName: "Home"
        }

        ClientPageModule.Client {
            id: clientPage
            objectName: "Client"
            visible: false
        }

        onCurrentItemChanged: {
            console.log("stackView current item: ", stackView.currentItem.objectName)
            if (appStack.currentItem == homePage)
                homePage.loaderClear()
        }
    }

    Connections {
        target: client
        function onSignalDisconnected() {
            if (!client.state && (stackView.currentItem != clientPage && stackView.currentItem != homePage))
                stackView.pop(homePage)
        }
    }

    Popup {
        id: popupWait
        objectName: "Popup"
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose
        visible: false

        background: Rectangle{
            opacity: 0.3
        }

        BusyIndicator {
            id: indicator
            antialiasing: true
            anchors.centerIn: parent
            visible: true
            running: true
        }
    }

    Loader {
        id: loaderDialog
        source: ""
    }


//    DialogShared {
//        id: dialogExit
//        parent: stackView
//        standardButtons: Dialog.Yes | Dialog.No
//        text: "Вы действительно хотите выйти?"

//        onAccepted: {
//            Qt.exit(0)
//        }
//    }

    Loader {
        id: loaderNotification
        source: ""
    }

    onClosing: {
        close.accepted = false
        if (appStack.currentItem == homePage)
        {
//            dialogExit.open()
            var obj = loaderDialog.setSource("qrc:/DialogShared.qml",
                                   {parent: appStack,
                                       visible: true,
                                       standardButtons: Dialog.Yes | Dialog.No,
                                       text: "Вы действительно хотите выйти?"})
            while(!loaderDialog.Ready)
                1

            if (loaderDialog.Ready)
                obj.accept.connect = function(){ Qt.exit(0) }
        }
        else
            appStack.pop()
    }
}

