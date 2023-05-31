import QtQuick 2.9
import QtQuick.Controls 2.5

Column {
    property string hostName: hostNameAction.hostName
    property string dataString: data.text
    property string dataType: dataTypeComboBox.currentText
    property int channelNumber: chNum.value

    width: parent.width

    HostNameComboBox {
        id: hostNameAction
        height: 50
        width: parent.width
    }

    Item {
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
        }
    }

    DataString {
        id: data
    }

    ChannelNumber {
        id: chNum
        label: "№ канала"
    }
}
