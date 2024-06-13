import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
    property variant arg

    onVisibleChanged: {
        updateListModel()
    }

    RoundButton {
        id: addNewActionGroup
        z: 1
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
            listView.loader.setSource("qrc:/Events/ActionsGroup/AddGroup.qml",
                                      {btnDeleteVisible: false})
            listView.loader.title =  "Добавить новую группу действий"
            listView.loader.objectName =  listView.loader.title
            glob_eventStackView.push(listView.loader)
        }
    }

    BaseItem.GridList {
        id: listView

        model: ListModel {id: listModel}

        Component.onCompleted: {
            updateListModel()
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

        for (var i = 0; i < list.length; i++)
        {
            var objectAtributes = {
                groupName: list[i]
            }

            var object = {
                text: list[i],
                title: list[i],
                icon: "qrc:/img/folder.png",
                loaderSource: "qrc:/Events/ActionsGroup/ActionsList.qml",
                attributes: [objectAtributes]
            }
            listModel.append(object)
        }
        listView.height = list.length * 100
    }

    function destroyEvent()
    {
        console.log("Actions group destroy");
//        if (actionslistLoader.item != null)
//            actionslistLoader.item.destroyEvent()
        //        timer.start()
        //        listView.destroyEv()
//        acEvlistLoader.setSource("")
    }
}