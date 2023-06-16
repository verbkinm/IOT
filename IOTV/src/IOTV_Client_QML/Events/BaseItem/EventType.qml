import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property string eventType: eventTypeComboBox.currentText
    property alias model: eventTypeComboBox.model
    property alias comboBox: eventTypeComboBox
    property string startType: ""

    signal signalActivated();

    id: eventTypeItem
    height: 50
    width: 400//parent.width

    Text {
        id: eventTypeNameText
        text: "Тип события:"
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    ComboBox {
        id: eventTypeComboBox
        width: 200

        model: ["connection", "disconnection", "state", "data"]
        currentIndex: startIndex(model, startType)

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
