import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property string hostName: hostComboBox.currentText
    property alias label: hostNameText.text

    id: root
    height: 50
    width: parent.width

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
}
