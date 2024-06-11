import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    required property var obj
    property string objType: objTypeComboBox.currentText
    property alias model: objTypeComboBox.model
    property alias comboBox: objTypeComboBox
    property string startType: ""

    signal signalActivated();

    id: eventTypeItem
    height: 50
    width: 400

    Text {
        text: "Тип:"
        width: parent.width / 2
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    ComboBox {
        id: objTypeComboBox
        width: parent.width / 2

        currentIndex: startIndex(model, obj.type)

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
