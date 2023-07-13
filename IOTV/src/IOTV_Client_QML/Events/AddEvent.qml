import QtQuick 2.9
import QtQuick.Controls 2.5

import "qrc:/Events/BaseItem" as BaseItem

Page {
    property var _event
    property var _action

    property alias btnDeleteVisible: deleteEvent.visible

    id: root
    title: "Новое событие"

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
            spacing: 15

            Item {
                height: 50
                width: parent.width

                Text {
                    id: eventTitle
                    text: "Событие:"
                    font.pixelSize: 16

                    anchors {
                        centerIn: parent
                        top: parent.top
                    }
                }
            }

            BaseItem.DataString {
                id: name
                width: parent.width
                label: "Название: "
                text: title
            }

            BaseItem.HostNameComboBox {
                id: hostNameItem
                height: 50
                width: parent.width
                startHostName: (_event === undefined) ? "" : _event["host_name"]
            }

            BaseItem.EventType {
                id: eventTypeItem
                height: 50
                width: parent.width
                startType: (_event === undefined) ? "" : _event["type"]

                onSignalActivated: {
                    if (eventType === model[4])
                        hostNameItem.visible = false
                    else
                        hostNameItem.visible = true

                    if (eventType === model[0] || eventType === model[1])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/EmptyItem.qml")
                    else if(eventType === model[2])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/StateType.qml", {width: parent.width})
                    else if(eventType === model[3])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/DataType.qml", {width: parent.width})
                    else if(eventType === model[4])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/AlarmType.qml", {width: parent.width})
                }
            }

            // Событие

            Loader {
                id: eventTypeLoader
                source: ""
                width: parent.width
            }

            BaseItem.HorizontLine {
                height: 20
                width: parent.width
            }

            // Действие

            Item {
                height: 50
                width: parent.width

                Text {
                    id: actionTitle
                    text: "Действие:"
                    font.pixelSize: 16

                    anchors {
                        centerIn: parent
                        top: parent.top
                    }
                }
            }

            BaseItem.ActionType {
                id: actionTypeItem
                width: parent.width
                startType: (_action === undefined) ? "" : _action["type"]

                onSignalActivated: {
                    if (actiontType === model[0])
                        actionTypeLoader.setSource("qrc:/Events/BaseItem/ActionDataTX.qml", {width: parent.width})
                    else if(actiontType === model[1])
                        actionTypeLoader.setSource("qrc:/Events/BaseItem/ActionDataTX_Ref.qml", {width: parent.width})
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

                Button {
                    id: deleteEvent
                    width: 140
                    height: 60
                    text: "Удалить"
                    flat: false
                    highlighted: true
                    anchors {
                        right: save.left
                        rightMargin: 20
                    }

                    onClicked: {
                        client.removeEventAction(title)
                        appStack.pop()
                    }
                }

                Button {
                    id: save
                    text: "Сохранить"
                    width: deleteEvent.width
                    height: deleteEvent.height
                    flat: false
                    highlighted: true

                    anchors {
                        right: parent.right
                        rightMargin: 20
                    }

                    onClicked: {
                        if (name.text === "")
                        {
                            loaderMainItem.setSource("qrc:/DialogShared.qml",
                                                     {parent: appStack,
                                                         visible: true,
                                                         title: "Внимание",
                                                         standardButtons: Dialog.Ok,
                                                         text: "Имя события не может быть пустым!"})
                            return
                        }

                        if (title !== name.text)
                        {
                            var list = client.evAcList()
                            for( var i = 0; i < list.length; i++)
                            {
                                var obj = list[i][0]
                                if (obj["name"] === name.text)
                                {
                                    loaderMainItem.setSource("qrc:/DialogShared.qml",
                                                             {parent: appStack,
                                                                 visible: true,
                                                                 title: "Внимание",
                                                                 standardButtons: Dialog.Ok,
                                                                 text: "Событие с таким именем уже существует"})
                                    return
                                }
                            }
                        }

                        // Событие
                        var event = new Map
                        event["name"] = name.text
                        event["host_name"] = hostNameItem.hostName()
                        event["type"] = eventTypeItem.eventType

                        if(event["type"] === eventTypeItem.model[2])
                            event["state"] = eventTypeLoader.item.stateType()

                        else if(event["type"] === eventTypeItem.model[3])
                        {
                            event["compare"] = eventTypeLoader.item.compare()
                            event["direction"] = eventTypeLoader.item.direction()
                            event["chNumber"] = eventTypeLoader.item.channelNumber
                            if (event["compare"] !== eventTypeLoader.item.compareTypeComboBox.model[6] && event["compare"] !== eventTypeLoader.item.compareTypeComboBox.model[7])
                                event["data"] = eventTypeLoader.item.dataString

                        }
                        else if(event["type"] === eventTypeItem.model[4])
                        {
                            event["time"] = eventTypeLoader.item.time()
                            event["days"] = eventTypeLoader.item.days
                            console.log(event["time"])
                            console.log(event["days"])
                        }

                        var action = new Map
                        action["type"] = actionTypeItem.actiontType
                        if (action["type"] === actionTypeItem.model[0])
                        {
                            action["host_name"] = actionTypeLoader.item.currentHostName()
                            action["data"] = actionTypeLoader.item.dataString
                            action["chNumber"] = actionTypeLoader.item.channelNumber
                        }
                        else if (action["type"] === actionTypeItem.model[1])
                        {
                            action["src_host"] = actionTypeLoader.item.srcHostNameText()
                            action["dst_host"] = actionTypeLoader.item.dstHostNameText()
                            action["src_chNumber"] = actionTypeLoader.item.srcChannelNumber
                            action["dst_chNumber"] = actionTypeLoader.item.dstChannelNumber
                        }

                        client.saveEventAction(event, action, title)
                        appStack.pop()
                        //                        console.log(event)
                        //                        for(var el in event)
                        //                            console.log(el, " = ", event[el])

                        //                        console.log()

                        //                        for(el in action)
                        //                            console.log(el, " = ", action[el])
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Add Events page construct: ", objectName)
        //        forceActiveFocus()
    }

    Component.onDestruction: {
        console.log("Add Events page destruct: ", objectName)
    }

    onFocusChanged: {
        eventTypeItem.signalActivated()
        actionTypeItem.signalActivated()

        if (_event === undefined)
        {
            name.text = ""
        }
        else
        {
            var eventType = _event["type"]
            if (eventType === "state")
                stateProperty()
            else if (eventType === "data")
                dataProperty()
            else if (eventType === "alarm")
                alarmPoperty()
        }

        if (_action === undefined)
        {

        }
        else
        {
            var actionType = _action["type"]
            if (actionType === "data_tx")
                dataTX()
            else if (actionType === "data_tx_ref")
                dataTX_Ref()
        }
    }

    function stateProperty()
    {
        var state = _event["state"]
        eventTypeLoader.setSource("qrc:/Events/BaseItem/StateType.qml", {startState: state})
    }

    function dataProperty()
    {
        var compare = _event["compare"] === undefined ? "" : _event["compare"]
        var direction = _event["direction"] === undefined ? "" : _event["direction"]
        var data = _event["data"] === undefined ? "" : _event["data"]
        var chNum = _event["chNumber"] === undefined ? "" : _event["chNumber"]

        eventTypeLoader.setSource("qrc:/Events/BaseItem/DataType.qml", {startCompare: compare, startDirection: direction, dataString: data, channelNumber: chNum})
    }

    function alarmPoperty()
    {
        var _days = _event["days"]
        var hours = _event["time"].split(':')[0]
        var minutes = _event["time"].split(':')[1]
        eventTypeLoader.setSource("qrc:/Events/BaseItem/AlarmType.qml", {days: _days, h: hours, m: minutes})
    }

    function dataTX()
    {
        var hostName = _action["host_name"]
        var data = _action["data"]
        var chNum = _action["chNumber"]
        actionTypeLoader.setSource("qrc:/Events/BaseItem/ActionDataTX.qml", {dataString: data, channelNumber: chNum, hostName: hostName})
    }

    function dataTX_Ref()
    {
        var srcHostName = _action["src_host"]
        var dstHostName = _action["dst_host"]
        var srcChNum = _action["src_chNumber"]
        var dstChNum = _action["dst_chNumber"]
        actionTypeLoader.setSource("qrc:/Events/BaseItem/ActionDataTX_Ref.qml", {srcHostName: srcHostName, dstHostName: dstHostName, srcChannelNumber: srcChNum, dstChannelNumber: dstChNum})
    }
}
