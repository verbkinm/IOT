import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/Devices/" as Devices
import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
    title: "Настройки событий и действий"


    BaseItem.GridList {
        id: listView

        model: ListModel {
            id: listModel

            ListElement {
                text: "События"
                title: "Группы событий"
                icon: "qrc:/img/folder.png"
                loaderSource: "qrc:/Events/EventsGroup/EventsGroup.qml"
//                attributes: [
//                   ListElement {}
//                ]
            }

            ListElement {
                text: "Действия"
                title: "Группы действий"
                icon: "qrc:/img/folder.png"
                loaderSource: "qrc:/Events/ActionsGroup/ActionsGroup.qml"
//                attributes: [
//                    ListElement {title: "title 2"; arg: "255"}
//                ]
            }
        }
    }

    Devices.BusyRect {
        id: popup
        anchors.fill: parent
        visible: !client.state
    }

    Component.onCompleted: {
        console.log("Events and Actions page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Events and Actions page destruct: ", objectName)
    }

    Connections {
        target: client
        function onSignalConnected()
        {
            //            loaderHome.setSource("DeviceListView.qml")
        }
        function onSignalDisconnected()
        {
            //            glob_deviceStackView.pop(homePage)
            //            loaderHome.setSource("Home_Connector.qml")
        }
    }

    function destroyEv()
    {
        //        timer.start()
        //        listView.destroyEv()
        //        acEvlistLoader.setSource("")
//        if (acEvlistLoader.item != null)
//            acEvlistLoader.item.destroyEvent()
    }
}
