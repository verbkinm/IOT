import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
    title: "Группы действий"
    property variant arg

    Loader {
        property string title
        id: actionslistLoader
        objectName: title
    }

    GridView {
        id: listView
        cellWidth: global_window.width / 2 - global_window.width * 0.1
        cellHeight: 110

        anchors {
            fill: parent
            leftMargin: parent.width * 0.1
            rightMargin: parent.width * 0.1
            topMargin: 15
        }

        model: ListModel {id: listModel}
        delegate : BaseItem.ButtonShadow {
            label {
                text: model.group
            }
            icon {
                source: "qrc:/img/folder.png";
            }

            onSignalClicked: {
                actionslistLoader.setSource("qrc:/Events/ActionsGroup/ActionsList.qml",
                                     {groupName: model.group})
                actionslistLoader.title = "Список действий в группе \"" + model.group + "\""
                glob_eventStackView.push(actionslistLoader)
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

            print(arg)
        }
    }

    Connections {
        target: client
        function onSignalEventAction() {
//            glob_eventStackView.pop(eventsPage)
//            timer.stop()
//            updateListModel()
        }

        function onSignalDisconnected() {
            glob_eventStackView.pop(eventsPage)
        }

//        function onCountDeviceChanged() {
//            onSignalDisconnected()
//        }
    }


    Component.onCompleted: {
        console.log("Actions group page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Actions group page destruct: ", objectName)
    }

    function updateListModel()
    {
        listModel.clear()
        var list = client.actionsGroupList()

        console.log("list size = ", list.length)
        for (var i = 0; i < list.length; i++)
        {
            var object = {
                group: list[i]
            }
            listModel.append(object)
        }
        listView.height = list.length * 100
    }

    function destroyEvent()
    {
        console.log("Actions group destroy");
        if (actionslistLoader.item != null)
            actionslistLoader.item.destroyEvent()
        //        timer.start()
        //        listView.destroyEv()
//        acEvlistLoader.setSource("")
    }
}
