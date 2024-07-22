import QtQuick 2.9
import QtQuick.Controls 2.5

import "qrc:/Events/BaseItem" as BaseItem

Page {
    required property var _action

    required property bool newAction

    QtObject {
        id:privateields
        property string oldActionName: ""
    }

    id: root
    //    title: _event.name

    Flickable {
        width: parent.width
        height: parent.height

        contentHeight: column.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            anchors.top: parent.top
            anchors.topMargin: 15
            spacing: 15

            BaseItem.DataString {
                id: name
                width: parent.width
                label: "Имя действия: "
                text: _action.name
                placeholderText: "Название действия"

                onSignalTextEdited: {
                    _action.name = text
//                    runAction.enabled = false
                }
            }

            BaseItem.OnOff {
                id: onOff
                width: parent.width
                obj: _action

                onSignalActivated: {
//                    runAction.enabled = false
                }
            }

            BaseItem.ObjType {
                id: actionTypeItem
                width: parent.width
                obj: _action
                // iotv_action.h
                model: ["NONE", "DATA_TX", "DATA_TX_REF"]

                onSignalActivated: {
                    client.deleteObject(_action);
                    _action = client.createEmptyAction(comboBox.model[comboBox.currentIndex], _action.name, _action.groupName)
                    actionTypeCreate(objType)

//                    runAction.enabled = false
                }

                Component.onCompleted: {
                    actionTypeCreate(objType)
                }
            }

            Loader {
                id: actionTypeLoader
                source: ""
                width: parent.width
            }

            BaseItem.HorizontLine {
                height: 20
                width: parent.width
            }

            Item {
                width: parent.width
                height: 70

                RoundButton {
                    id: editAction
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        left: parent.left
                        leftMargin: 20
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/edit_white.png"
                        height: 24
                        width: 24
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        objectEnable(true)
                        runAction.enabled = false
                        enabled = false
                    }
                }

                RoundButton {
                    id: runAction
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        right: deleteAction.left
                        rightMargin: 20
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/id_4/play_white.png"
                        height: 24
                        width: 24
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        client.runAction(_action.groupName, _action.name)
                    }
                }

                RoundButton {
                    id: deleteAction
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        right: saveAction.left
                        rightMargin: 20
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/delete_white.png"
                        height: 24
                        width: 24
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        client.removeAction(_action.groupName, privateields.oldActionName)
                        glob_eventStackView.pop()
                    }
                }

                RoundButton {
                    id: saveAction
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        right: parent.right
                        rightMargin: 20
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/save_white.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        if (_action.name === "")
                        {
                            glob_dialogShared.defaultAcceptedMessage()
                            glob_dialogShared.title = "Внимание"
                            glob_dialogShared.text = "Имя события не может быть пустым!"
                            glob_dialogShared.open()
                            return
                        }

                        // Если добавляется новое действие
                        if (newAction)
                        {
                            if (client.isExistsActionNameInGroup(_action.groupName, _action.name))
                            {
                                actionExist()
                                return
                            }
                        }
                        // Если изменяется существующее действие
                        else if (!newAction)
                        {
                            if (_action.name !==  privateields.oldActionName && client.isExistsActionNameInGroup(_action.groupName, _action.name))
                            {
                                actionExist()
                                return
                            }

                            client.removeAction(_action.groupName, privateields.oldActionName)
                        }
                        client.saveAction(_action)
                        glob_eventStackView.pop()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Add Action page construct: ")
        privateields.oldActionName = _action.name

        if (newAction)
        {
            editAction.visible = false
            runAction.visible = false
            deleteAction.visible = false
        }
        else
            objectEnable(false)
    }

    Component.onDestruction: {
        console.log("Add Action page destruct: ", objectName)
        client.deleteObject(_action);
    }

    onFocusChanged: {
        //        eventTypeItem.signalActivated()
        ////        actionTypeItem.signalActivated()

        //        focus = true
    }

    function objectEnable(state)
    {
        name.enabled = state
        onOff.enabled = state
        actionTypeItem.enabled = state
        actionTypeLoader.enabled = state
        saveAction.enabled = state
    }

    function actionExist()
    {
        glob_dialogShared.defaultAcceptedMessage()
        glob_dialogShared.title = "Внимание"
        glob_dialogShared.text = "Действие с таким именем в группе \"" + _action.groupName + "\" уже существует!"
        glob_dialogShared.open()
    }

    function actionTypeCreate(eventType)
    {
        var model = actionTypeItem.model

        if (eventType === model[0])
            actionTypeLoader.setSource("qrc:/Events/BaseItem/EmptyItem.qml")
        else if(eventType === model[1])
            actionTypeLoader.setSource("qrc:/Events/BaseItem/ActionDataTX.qml", {action: _action, width: parent.width})
        else if(eventType === model[2])
            actionTypeLoader.setSource("qrc:/Events/BaseItem/ActionDataTX_Ref.qml", {action: _action, width: parent.width})
    }
}
