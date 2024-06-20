import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/Devices/" as Devices
import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
    title: "Настройки событий и действий"

    BaseItem.GridList {
        id: listView
        model: ListModel { id: listModel }
    }

    Devices.BusyRect {
        id: popup
        anchors.fill: parent
        visible: !client.state
    }

    Component.onCompleted: {
        console.log("Events and Actions page construct: ", objectName)

        updateListModel()
    }

    Component.onDestruction: {
        console.log("Events and Actions page destruct: ", objectName)
    }

    function updateListModel()
    {
        listModel.clear()

        var object1 = {
            text: "События",
            title: "Группы событий",
            icon: "qrc:/img/folder_events.png",
            loaderSource: "qrc:/Events/EventsGroup/EventsGroup.qml",
            stackView: glob_eventStackView,
        }
        listModel.append(object1)

        var object2 = {
            text: "Действия",
            title: "Группы действий",
            icon: "qrc:/img/folder_actions.png",
            loaderSource: "qrc:/Events/ActionsGroup/ActionsGroup.qml",
            stackView: glob_eventStackView,
        }
        listModel.append(object2)

        listView.height = 2 * 100
    }

    function destroyEv()
    {

    }
}
