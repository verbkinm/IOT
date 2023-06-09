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
        forceActiveFocus()
    }

    Component.onDestruction: {
        console.log("Add Events page destruct: ", objectName)
    }

    onFocusChanged: {
        eventTypeItem.signalActivated()
        actionTypeItem.signalActivated()

        var eventType = _event["type"]

        if (eventType === "state")
            stateProperty()
        else if (eventType === "data")
            dataProperty()
    }

    function stateProperty()
    {
        eventTypeLoader.setSource("qrc:/Events/BaseItem/StateType.qml", {width: column.width})
        var state = _event["state"]
        var index = eventTypeLoader.item.comboBox.find(state)

        if (index === -1)
        {
            eventTypeLoader.item.enabled = false
            eventTypeLoader.item.model = [state]
        }
        else
        {
            eventTypeLoader.item.comboBox.currentIndex = index
        }
    }

    function dataProperty()
    {
        eventTypeLoader.setSource("qrc:/Events/BaseItem/DataType.qml", {width: column.width})
        var item = eventTypeLoader.item

        // direction
        var direction = _event["direction"]
        var index = item.directectionComboBox.find(direction)

        if (index === -1)
        {
            item.enabled = false
            item.directectionComboBox.model = [direction]
        }
        else
        {
            item.directectionComboBox.currentIndex = index
        }

        // compare
        var compare = _event["compare"]
        index = eventTypeLoader.item.compareTypeComboBox.find(compare)

        if (index === -1)
        {
            item.enabled = false
            item.compareTypeComboBox.model = [compare]
        }
        else
        {
            item.compareTypeComboBox.currentIndex = index
        }

        if (compare !== "always true" && compare !== "always false")
        {
            // dataType
            var dataType = _event["data_type"]
            index = item.dataTypeComboBox.find(dataType)

            if (index === -1)
            {
                item.enabled = false
                item.dataTypeComboBox.model = [dataType]
            }
            else
            {
                item.dataTypeComboBox.currentIndex = index
            }

            // data
            var data = _event["data"]
            item.dataString = data
        }

        // channel number
        var chNum = _event["chNumber"]
        item.channelNumber = chNum
    }
}
