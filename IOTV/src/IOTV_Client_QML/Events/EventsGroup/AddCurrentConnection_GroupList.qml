import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
    required property var event

    onVisibleChanged: {
        updateListModel()
    }

    BaseItem.GridList {
        id: listView

        model: ListModel {id: listModel}

        Component.onCompleted: {
            updateListModel()
        }
    }

    function updateListModel()
    {
        listModel.clear()
        var list = client.actionsGroupList()

        for (var i = 0; i < list.length; i++)
        {
            var objectAtributes = {
                groupName: list[i],
                event: root.event
            }

            var object = {
                text: list[i],
                title: list[i],
                icon: "qrc:/img/folder.png",
                loaderSource: "qrc:/Events/EventsGroup/AddCurrentConnection_ActionList.qml",
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
