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
                        {
                            glob_swipeView.setCurrentIndex(0)
                            glob_deviceStackView.clear()
                            glob_deviceStackView.push(homePage)
                        }
                        else if (index === 1)
                        {
                            glob_swipeView.setCurrentIndex(2)
                            glob_clientStackView.pop(clientPage)
                        }
                        else if (index === 2)
                        {
                            glob_dialogShared.defaultAcceptedMessage()
                            glob_dialogShared.title = "Клиент IOTV"
                            glob_dialogShared.text = "Версия: " + Qt.application.version
                            glob_dialogShared.open()
                        }
                        else if (index === 3)
                        {
                            glob_dialogShared.defaultAcceptedExit()
                            glob_dialogShared.open()
                        }

                        drawer.close()
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
