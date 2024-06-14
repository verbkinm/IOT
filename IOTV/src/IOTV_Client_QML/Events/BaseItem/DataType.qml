import QtQuick 2.9
import QtQuick.Controls 2.5

Column {
    required property var event

    id: root
    width: 400
    spacing: 15

    Item {
        id: rectData_1
        height: 50
        width: parent.width

        Text {
            text: "Направление:"
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 20
            }
        }

        ComboBox {
            id: direction
            width: parent.width / 2

            //iotv_event_data.h
            model: ["NONE", "RX", "TX", "ANY", "CHANGE"]
            currentIndex: startIndex(model, event.directionStr)

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 20
            }

            onCurrentIndexChanged: {
                event.directionStr = direction.model[currentIndex]
            }
        }
    }

    Item {
        height: 50
        width: parent.width

        Text {
            text: "Условие:"
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 20
            }
        }

        ComboBox {
            id: compareType
            width: parent.width / 2

            // ConfigTypes.h struct Json_Event_Action
            model: ["==", "!=", ">", "<", ">=", "<=", "always true", "always false"]
            currentIndex: startIndex(model, event.compare)

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 20
            }

            onCurrentIndexChanged: {
                event.compare = compareType.model[currentIndex]
            }
        }
    }

    DataString {
        id: _data
        visible: !((compareType.currentText === compareType.model[6]) || (compareType.currentText === compareType.model[7]))
        width: parent.width
        text: event.dataStr

        onSignalTextEdited: {
            event.dataStr = text
        }
    }

    ChannelNumber {
        id: chNum
        label: "№ канала"
        width: parent.width
        value: event.chNum

        onSignalNumberChanged: {
            event.chNum = value
        }
    }

    Component.onCompleted: {
    }

    function startIndex(model, textItem)
    {
        if (textItem === "")
            return 0;

        const found = (element) => element === textItem;
        var index = model.findIndex(found)

        return index === -1 ? 0 : index
    }
}
