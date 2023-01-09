import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

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
    property bool exit: false

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
                stackView.pop()
            }
            visible: !homePage.visible
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
                                //                                stackView.pop(homePage)
                                stackView.push(clientPage)
                            }
                            else if (index === 2)
                            {
                                //                                about.visible = true
                                about.open()
                            }

                            else if (index === 3)
                            {
                                exit = true
                                Qt.quit()
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

        onCurrentItemChanged: {
            console.log("stackView current item: ", stackView.currentItem.objectName)
//            if (stackView.currentItem.objectName == homePage.objectName)
//            {
//                for (var i = stackView.children.length - 1; i > 1; i--)
//                    stackView.children[i].destroy()
//            }
        }

        Home {
            id: homePage
            objectName: "Home"
        }

        Client {
            id: clientPage
            objectName: "Client"
            visible: false
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
        visible: clientPage.connection_attempt

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

    Dialog {
        id: about
        modal: true
        parent: stackView

        title: "О программе"
        standardButtons: Dialog.Ok

        width: parent.width * 0.8
        height: parent.height * 0.5
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)

        Label {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "Клиент IOTV\n Версия " + Qt.application.version
            font.pixelSize: 18
            wrapMode: Text.Wrap
        }
    }

    Dialog {
        id: connectionError
        modal: true
        parent: stackView

        title: "Ошибка"
        standardButtons: Dialog.Ok

        width: parent.width * 0.8
        height: parent.height * 0.5
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)

        Label {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "Не удалось подключиться к серверу."
            font.pixelSize: 18
            wrapMode: Text.Wrap
        }
    }
}

