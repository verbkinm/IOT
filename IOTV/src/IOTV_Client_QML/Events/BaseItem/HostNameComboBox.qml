import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property string startText: ""
    property alias text: hostNameText.text

    id: root
    height: 50
    width: 400

    signal signalCurrentTextChanged(var modelIndexText)

    Text {
        id: hostNameText
        text: "Имя устройства:"
        width: parent.width / 2

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    ComboBox {
        id: hostComboBox
        width: parent.width / 2

        model: client.allHostAliasName()
        currentIndex: startIndex(model, startText)

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }

        onCurrentIndexChanged: {
            signalCurrentTextChanged(hostComboBox.model[currentIndex])
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
