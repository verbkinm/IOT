import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
//    property string hostName: hostComboBox.currentText
//    property alias label: hostNameText.text
//    property alias model: hostComboBox.model
    property alias comboBox: hostComboBox

    property string startHostName: ""

    id: root
    height: 50
    width: 400//parent.width

    Text {
        id: hostNameText
        text: "Имя устройства:"

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    ComboBox {
        id: hostComboBox
        width: 200

        model: listName()
        currentIndex: startIndex(model, startHostName)

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }
    }

    function listName() {
        var arr = []
        for( var i = 0; i < client.devList().length; i++)
            arr.push(client.devList()[i].aliasName)

        return arr
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
