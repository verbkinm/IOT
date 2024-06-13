import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem
import "qrc:/Events/ActionsGroup/" as ActionsGroup

Page {
    id: root

    required property var event
    required property string groupName

    RoundButton {
        id: saveActions
        z:1
        width: 64
        height: 64
        highlighted: true
        enabled: false


        Image {
            anchors.centerIn: parent
            source: "qrc:/img/save_white.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit
        }

        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 10
            bottomMargin: 10
        }

        onClicked: {
            for (var i = 0; i < listModel.count; i++)
            {
                if (listModel.get(i).checked)
                    event.slotAddAction(groupName, listModel.get(i).text)
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
                saveActionButtonEnable()
            }
        }

        Component.onCompleted: {
            updateListModel()
        }
    }

    function saveActionButtonEnable()
    {
        for (var i = 0; i < listModel.count; i++)
        {
            if (listModel.get(i).checked)
            {
                saveActions.enabled = true
                return
            }
        }

        saveActions.enabled = false
    }

    function updateListModel()
    {
        listModel.clear()
        var list = client.actionsListInGroup(root.groupName)
        for (var i = 0; i < list.length; i++)
        {
            var action = client.copyActionByNameAndGroup(list[i], root.groupName)

            var object = {
                checked: false,
                text: list[i],
                icon: actionImageByEventType(action.type),
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
