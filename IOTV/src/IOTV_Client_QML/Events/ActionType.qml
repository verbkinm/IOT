import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property string actiontType: actionTypeComboBox.currentText
    property alias model: actionTypeComboBox.model

    id: actionTypeItem
    height: 50
    width: parent.width

    Text {
        text: "Тип действия:"

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    ComboBox {
        id: actionTypeComboBox
        width: 200

        model: ["data_tx", "data_tx_ref"]

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }
    }
}
