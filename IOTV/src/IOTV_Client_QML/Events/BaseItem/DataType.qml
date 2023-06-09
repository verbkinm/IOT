import QtQuick 2.9
import QtQuick.Controls 2.5

Column {
    property alias directectionComboBox: directionTypeComboBox
    property alias compareTypeComboBox: compareTypeComboBox
    property alias dataTypeComboBox: dataTypeComboBox
    property alias dataString: _data.text
    property alias channelNumber: chNum.value


    //    property alias model: compareTypeComboBox.model

    id: root
    width: 400//parent.width

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

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 20
            }
        }
    }

    Item {
        visible: !((compareTypeComboBox.currentText === compareTypeComboBox.model[6]) || (compareTypeComboBox.currentText === compareTypeComboBox.model[7]))
        height: 50
        width: parent.width

        Text {
            id: dataTypeText
            text: "Тип данных:"
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 20
            }
        }

        ComboBox {
            id: dataTypeComboBox
            width: 200
            model: ["int_8", "int_16", "int_32", "int_64", "float_32", "double_64", "bool", "string", "raw"]

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 20
            }

            onActivated: {
                console.log (compareTypeComboBox.currentText)
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
}
