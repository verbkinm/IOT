import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property string _state: stateTypeComboBox.currentText
    property alias model: stateTypeComboBox.model

    id: itemStateType
    height: 50
    width: parent.width

    Text {
        text: "Тип состояния:"
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    ComboBox {
        id: stateTypeComboBox
        width: 200

        model: ["online", "offline", "switch"]

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }
    }
}
