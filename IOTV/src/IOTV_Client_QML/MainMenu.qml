import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

import "Home" as HomePageModule
import "Client" as ClientPageModule

Drawer {
    id: drawer

    width: global_window.width / 2
    height: global_window.height - overlayHeader.height

    modal: inPortrait
//    interactive: inPortrait
//    position: inPortrait ? 0 : 1
//    visible: !inPortrait


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
                            appStack.pop(homePage)
                        else if (index === 1 && appStack.currentItem != clientPage)
                            appStack.push(clientPage)
                        else if (index === 2)
                        {
                            loaderMainItem.setSource("DialogShared.qml",
                                                     {parent: appStack,
                                                         visible: true,
                                                         standardButtons: Dialog.Ok,
                                                         title: "Версия",
                                                         text: "Клиент IOTV " + Qt.application.version})
                        }

                        else if (index === 3)
                            dialogExit.open()

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
