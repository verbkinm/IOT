import QtQuick 2.9
import QtQuick.Controls 2.5

Column {
    property alias directectionComboBox: directionTypeComboBox
    property alias compareTypeComboBox: compareTypeComboBox
    property alias dataString: _data.text
    property alias channelNumber: chNum.value

    property string startDirection: ""
    property string startCompare: ""

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
            id: directionTypeComboBox
            width: 200

            model: ["tx", "rx", "any", "change"]
            currentIndex: startIndex(model, startDirection)

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 20
            }
        }
    }

    Item {
        height: 50
        width: parent.width

        Text {
            id: conditionText
            text: "Условие:"
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 20
            }
        }

        ComboBox {
            id: compareTypeComboBox
            width: 200

            model: ["==", "!=", ">", "<", ">=", "<=", "always true", "always false"]
            currentIndex: startIndex(model, startCompare)

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 20
            }
        }
    }

    DataString {
        id: _data
        visible: !((compareTypeComboBox.currentText === compareTypeComboBox.model[6]) || (compareTypeComboBox.currentText === compareTypeComboBox.model[7]))
        width: parent.width
    }

    ChannelNumber {
        id: chNum
        label: "№ канала"
        width: parent.width
    }

    function startIndex(model, textItem)
    {
        if (textItem === "")
            return 0;

        const found = (element) => element === textItem;
        var index = model.findIndex(found)

        return index === -1 ? 0 : index
    }

    function compare() {
        return compareTypeComboBox.currentText
    }

    function direction() {
        return directionTypeComboBox.currentText
    }
}
