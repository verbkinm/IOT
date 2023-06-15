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
                text: _event["name"]
            }

            BaseItem.HostNameComboBox {
                id: hostNameItem
                height: 50
                width: parent.width
                startHostName: _event["host_name"]
            }

            BaseItem.EventType {
                id: eventTypeItem
                height: 50
                width: parent.width


                onSignalActivated: {
                    if (eventType === model[0] || eventType === model[1])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/EmptyItem.qml")
                    else if(eventType === model[2])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/StateType.qml", {width: parent.width})
                    else if(eventType === model[3])
                        eventTypeLoader.setSource("qrc:/Events/BaseItem/DataType.qml", {width: parent.width})
                }

                Component.onCompleted: {
                    var eventType = _event["type"]
                    var index = comboBox.find(eventType)
                    comboBox.currentIndex = index
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

//        var hostName  = _event["host_name"]
//        var index = hostNameItem.comboBox.find(hostName)

//        if (index === -1)
//        {
//            hostNameItem.enabled = false
//            hostNameItem.model = [hostName]
//        }
//        else
//        {
//            hostNameItem.comboBox.currentIndex = index
//        }

        var eventType = _event["type"]
        if (eventType === "state")
            stateProperty()
        else if (eventType === "data")
            dataProperty()
    }

    function stateProperty()
    {
        var state = _event["state"]
        eventTypeLoader.setSource("qrc:/Events/BaseItem/StateType.qml", {startState: state})
    }

    function dataProperty()
    {
        var compare = _event["compare"]
        var direction = _event["direction"]
        var data = _event["data"]
        var chNum = _event["chNumber"]

        eventTypeLoader.setSource("qrc:/Events/BaseItem/DataType.qml", {startCompare: compare, startDirection: direction, dataString: data, channelNumber: chNum})
    }
}
