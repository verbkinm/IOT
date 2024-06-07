import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
    title: "Список действий в группе"

    property string groupName: ""

    Loader {
        property string title
        id: eventLoader
        objectName: title
    }

    ListView {
        id: listView
        anchors {
            fill: parent
            leftMargin: 20
            rightMargin: 20
            topMargin: 20
        }
        spacing: 20

        model: ListModel {id: listModel}
        delegate : BaseItem.ButtonShadow {

            width: parent.width
            label {
                text: model.eventName
            }
            onSignalClicked: {
                eventLoader.setSource("qrc:/Events/ActionsGroup/EvenAction.qml")
//                                     {_event: model.event, _action: model.action, title: model.name})
//                acEvLoader.title = model.name
                glob_eventStackView.push(eventLoader)
            }
        }

        // Анимация появления элементов модели
        populate: Transition {
            NumberAnimation { properties: "x,y"; duration: 1000; easing.type: Easing.OutExpo }
        }
        // Анимация добавления элементов
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 500 }
            NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 500; alwaysRunToEnd: true }
        }
        // Удаление элемента
        remove: Transition {
            PropertyAnimation{ property: "opacity"; to: 0; duration: 500}
            PropertyAnimation{ property: "scale"; to: 0; duration: 500; alwaysRunToEnd: true}
        }

        Component.onCompleted: {
            updateListModel()
        }
    }

    Component.onCompleted: {
        console.log("Actions list page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Actions list page destruct: ", objectName)
    }

    function updateListModel()
    {
        listModel.clear()
        var list = client.actionsListInGroup(root.groupName)

        console.log("list size = ", list.length)
        for (var i = 0; i < list.length; i++)
        {
            var object = {
                eventName: list[i]
            }
            listModel.append(object)
        }
        listView.height = list.length * 100
    }

    function destroyEvent()
    {
        console.log("Events list destroy");
        if (eventLoader.item != null)
            eventLoader.item.destroyEvent()
        //        timer.start()
        //        listView.destroyEv()
//        acEvlistLoader.setSource("")
    }
}
