import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
    title: "Список событий в группе"

    property string groupName: ""

    onVisibleChanged: {
        updateListModel()
    }

    RoundButton {
        id: addNewEvent
        z:1
        width: 64
        height: 64
        highlighted: true

        text: "+"
        font.pixelSize: 36

        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 10
            bottomMargin: 10
        }

        onClicked: {
            listView.loader.setSource("qrc:/Events/EventsGroup/Event.qml",
                                 { _event: client.createEmptyEvent("NONE", "Новое событие", groupName), btnDeleteVisible: false})
            listView.loader.title =  "Новое событие"
            listView.loader.objectName =  listView.loader.title
            glob_eventStackView.push(listView.loader)
        }
    }

    EventGroupHeader {
        id:eventGroupHeader
        text: groupName
        icon: "qrc:/img/folder.png"

        onSignalClicked: {
            listView.loader.setSource("qrc:/Events/EventsGroup/AddGroup.qml",
                                      {btnDeleteVisible: true, oldGroupName: groupName})
            listView.loader.title = groupName
            listView.loader.objectName = listView.loader.title
            glob_eventStackView.push(listView.loader)
        }
    }

    BaseItem.GridList  {
        id: listView

        anchors.topMargin: eventGroupHeader.height + 20

        model: ListModel {id: listModel}

        Component.onCompleted: {
            updateListModel()
        }   
    }

    Component.onCompleted: {
        console.log("Events list page construct: ", objectName)
        print("groupName = ", groupName)
    }

    Component.onDestruction: {
        console.log("Events list page destruct: ", objectName)
    }

    function updateListModel()
    {
        listModel.clear()
        var list = client.eventsListInGroup(root.groupName)

        console.log("events in group list size = ", list.length)
        for (var i = 0; i < list.length; i++)
        {
            var objectAtributes = {
                _event: client.copyEventByNameAndGroup(list[i], root.groupName),
                btnDeleteVisible: true
            }

            var object = {
                text: list[i],
                title: list[i],
                icon: actionImageByEventType(objectAtributes._event.type),
                loaderSource: "qrc:/Events/EventsGroup/Event.qml",
                attributes: [objectAtributes]
            }
            listModel.append(object)
        }
        listView.height = list.length * 100
    }

    function actionImageByEventType(eventType) {

        // iotv_event.h
        switch (eventType)
        {
        case "CONNECTING":
            return "qrc:/img/events/connected.png"
        case "DISCONNECTING":
            return "qrc:/img/events/disconnected.png"
        case "STATE":
            return "qrc:/img/events/state.png"
        case "DATA":
            return "qrc:/img/events/data.png"
        case "ALARM":
            return "qrc:/img/events/alarm.png"
        case "TIMER":
            return "qrc:/img/events/timer.png"
        }

        return "qrc:/img/id/0.png"
    }

    function destroyEvent()
    {
        console.log("Events list destroy");
//        if (eventLoader.item != null)
//            eventLoader.item.destroyEvent()
        //        timer.start()
        //        listView.destroyEv()
//        acEvlistLoader.setSource("")
    }
}
