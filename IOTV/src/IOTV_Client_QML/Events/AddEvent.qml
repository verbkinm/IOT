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
            anchors.topMargin: 15
            spacing: 15

            BaseItem.DataString {
                id: name
                width: parent.width
                label: "Имя событие: "
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
                    if (eventType === model[4] || eventType === model[5])
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
                    else if(eventType === model[5])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/TimerType.qml", {width: parent.width})
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

                RoundButton {
                    id: deleteEvent
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        right: save.left
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
                        client.removeEventAction(title)
                        glob_eventStackView.pop(eventsPage)
                    }
                }

                RoundButton {
                    id: save
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
                        if (name.text === "")
                        {
                            glob_dialogShared.defaultAcceptedMessage()
                            glob_dialogShared.title = "Внимание"
                            glob_dialogShared.text = "Имя события не может быть пустым!"
                            glob_dialogShared.open()
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
                                    glob_dialogShared.defaultAcceptedMessage()
                                    glob_dialogShared.title = "Внимание"
                                    glob_dialogShared.text = "Событие с таким именем уже существует!"
                                    glob_dialogShared.open()
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
                        }
                        else if(event["type"] === eventTypeItem.model[5])
                        {
                            event["seconds"] = eventTypeLoader.item.getTotalSeconds()
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
                        client.queryEventAction()

                        glob_eventStackView.pop(eventsPage)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Add Events page construct: ", objectName)
        focusChanged(true)
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
            else if (eventType === "timer")
                timerPoperty()
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

        focus = true
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

    function timerPoperty()
    {
        var _seconds = _event["seconds"]
        eventTypeLoader.setSource("qrc:/Events/BaseItem/TimerType.qml", {totalSeconds: _seconds})
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
