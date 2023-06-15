import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias comboBox: stateTypeComboBox
    property string startState: ""

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

        model: ["online", "offline", "switch"]
        currentIndex: startIndex(model, startState)

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
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
