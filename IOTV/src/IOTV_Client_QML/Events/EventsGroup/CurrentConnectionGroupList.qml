import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem
import "qrc:/Events/ActionsGroup/" as ActionsGroup

Page {
    id: root
    //    title: "Список действий в группе"

    required property var event

    onVisibleChanged: {
        updateListModel()
    }

    RoundButton {
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
            listView.loader.setSource("qrc:/Events/EventsGroup/AddCurrentConnection_GroupList.qml",
                                      { event: root.event})
            listView.loader.title =  "Группы событий"
            listView.loader.objectName =  listView.loader.title
            glob_eventStackView.push(listView.loader)
        }
    }

    ActionsGroup.ActionGroupHeader {
        id: actionGroupHeader
        text: event.name
        icon: imageByEventType(event.type)
        readOnly: true
    }

    BaseItem.GridList  {
        id: listView

        anchors.topMargin: actionGroupHeader.height + 20

        model: ListModel {id: listModel}

        Component.onCompleted: {
            updateListModel()
        }
    }

    Component.onCompleted: {
        console.log("CurrentConnectionList page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("CurrentConnectionList page destruct: ", objectName)
    }

    function updateListModel()
    {
        listModel.clear()
        var list = event.actionGroups

        for (var i = 0; i < list.length; i++)
        {
            var objectAtributes = {
                groupName: list[i],
                event: root.event
            }

            var object = {
                text: list[i],
                title: "Соединения",
                icon: "qrc:/img/folder.png",
                loaderSource: "qrc:/Events/EventsGroup/CurrentConnectionList.qml",
                attributes: [objectAtributes]
            }
            listModel.append(object)
        }
        listView.height = list.length * 100
    }
}
