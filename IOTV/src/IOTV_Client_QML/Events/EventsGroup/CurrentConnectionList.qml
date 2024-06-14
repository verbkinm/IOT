import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem
import "qrc:/Events/ActionsGroup/" as ActionsGroup

Page {
    id: root

    required property var event
    required property string groupName

    RoundButton {
        id: delActions
        z:1
        width: 64
        height: 64
        highlighted: true
        enabled: false


        font.pixelSize: 36

        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 10
            bottomMargin: 10
        }

        Image {
            anchors.centerIn: parent
            source: "qrc:/img/delete_white.png"
            height: 24
            width: 24
            fillMode: Image.PreserveAspectFit
        }

        onClicked: {
            print("delete action: ")
            for (var i = 0; i < listModel.count; i++)
            {
                if (listModel.get(i).checked)
                {
                    var actionName = listModel.get(i).text
                    event.slotRemoveAction(groupName, actionName)
                }
            }
            client.saveEvent(_event)
            glob_eventStackView.pop()
        }
    }

    ActionsGroup.ActionGroupHeader {
        id: actionGroupHeader
        text: event.name + " / " + groupName
        icon: imageByEventType(event.type)
        readOnly: true
    }

    BaseItem.GridList  {
        id: listView

        anchors.topMargin: actionGroupHeader.height + 20

        model: ListModel {id: listModel}

        delegate : BaseItem.ButtonShadow {
            checkable: true
            label { text: model.text }
            icon { source: model.icon }

            onSignalToggled: {
                model.checked = this.checked
                deleteActionButtonEnable()
            }
        }

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

    function deleteActionButtonEnable()
    {
        for (var i = 0; i < listModel.count; i++)
        {
            if (listModel.get(i).checked)
            {
                delActions.enabled = true
                return
            }
        }

        delActions.enabled = false
    }

    function updateListModel()
    {
        listModel.clear()
        var list = event.slotActionInGroup(root.groupName)
        for (var i = 0; i < list.length; i++)
        {
            var objectAtributes = {
                _action: client.copyActionByNameAndGroup(list[i], root.groupName),
//                btnDeleteVisible: true
            }
            var object = {
                text: list[i],
                title: list[i],
                checked: false,
                icon: actionImageByEventType(objectAtributes._action.type),
//                loaderSource: "qrc:/Events/ActionsGroup/Action.qml",
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
}
