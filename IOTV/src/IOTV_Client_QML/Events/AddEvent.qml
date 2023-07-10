import QtQuick 2.9
import QtQuick.Controls 2.5

import "qrc:/Events/BaseItem" as BaseItem

Page {
    property var _event
    property var _action

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
                    if (eventType === model[0] || eventType === model[1])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/EmptyItem.qml")
                    else if(eventType === model[2])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/StateType.qml", {width: parent.width})
                    else if(eventType === model[3])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/DataType.qml", {width: parent.width})
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
                height: 50
                Button {
                    id: save
                    text: "Сохранить"
                    anchors {
                        right: parent.right
                        rightMargin: 20
                        verticalCenter: parent.verticalCenter
                    }

                    onClicked: {

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
