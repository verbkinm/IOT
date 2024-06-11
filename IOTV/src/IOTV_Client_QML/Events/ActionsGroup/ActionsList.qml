import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
//    title: "Список действий в группе"

    property string groupName: ""

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
            listView.loader.setSource("qrc:/Events/ActionsGroup/Action.qml",
                                 { _action: client.createEmptyAction("NONE", "Новое действие", groupName), btnDeleteVisible: false})
            listView.loader.title =  "Новое действие"
            listView.loader.objectName =  listView.loader.title
            glob_eventStackView.push(listView.loader)
        }
    }

    ActionGroupHeader {
        id: actionGroupHeader
        text: groupName
        icon: "qrc:/img/folder.png"

        onSignalClicked: {
            listView.loader.setSource("qrc:/Events/ActionsGroup/AddGroup.qml",
                                      {btnDeleteVisible: true, oldGroupName: groupName})
            listView.loader.title = groupName
            listView.loader.objectName = listView.loader.title
            glob_eventStackView.push(listView.loader)
        }
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
        console.log("Actions list page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Actions list page destruct: ", objectName)
    }

    function updateListModel()
    {
        listModel.clear()
        var list = client.actionsListInGroup(root.groupName)
        for (var i = 0; i < list.length; i++)
        {
            var objectAtributes = {
                _action: client.copyActionByNameAndGroup(list[i], root.groupName),
                btnDeleteVisible: true
            }
            var object = {
                text: list[i],
                title: list[i],
                icon: actionImageByEventType(objectAtributes._action.type),
                loaderSource: "qrc:/Events/ActionsGroup/Action.qml",
                attributes: [objectAtributes]
            }
            listModel.append(object)
        }
        listView.height = list.length * 100
    }

    function actionImageByEventType(actionType) {
        // iotv_action.h
        switch (actionType)
        {
        case "DATA_TX":
            return "qrc:/img/actions/data_tx.png"
        case "DATA_TX_REF":
            return "qrc:/img/actions/data_tx_ref.png"
        }

        return "qrc:/img/id/0.png"
    }

    function destroyEvent()
    {
        console.log("Actions list destroy");
//        if (eventLoader.item != null)
//            eventLoader.item.destroyEvent()
        //        timer.start()
        //        listView.destroyEv()
//        acEvlistLoader.setSource("")
    }
}
