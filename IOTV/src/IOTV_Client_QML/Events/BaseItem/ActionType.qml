import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    required property var action
    property string actionType: actionTypeComboBox.currentText
    property alias model: actionTypeComboBox.model
    property alias comboBox: actionTypeComboBox
    property string startType: ""

    signal signalActivated();

    height: 50
    width: 400//parent.width

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

        // iotv_action.h
        model: ["NONE", "NONE", "DATA_TX", "DATA_TX_REF"]
        currentIndex: startIndex(model, action.type)

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }

        onActivated: {
            signalActivated();
        }
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
