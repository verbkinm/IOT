import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    required property var event

    id: itemStateType
    height: 50
    width: 400

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

        // iotv_event_state.h
        model: ["NONE", "ONLINE", "OFFLINE", "SWITCH"]
        currentIndex: startIndex(model, event.state)

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }

        onCurrentIndexChanged: {
            event.state = currentText
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

    function stateType() {
        return stateTypeComboBox.currentText
    }
}
