import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias text: txtField.text
    property int number: 0
    property alias type: typeName.text
    property alias button: btn

    height: 50
    width: parent.width

    Label {
        id: lb1
        text: number + ":"
        font.pixelSize: 12
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: 10
            rightMargin: 10
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Label {
        id: typeName
        text: "type"
        font.pixelSize: 12
        anchors {
            left: lb1.right
            top: parent.top
            bottom: parent.bottom
            leftMargin: 10
            rightMargin: 10
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    TextField {
        id: txtField
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        font.pixelSize: 12
        readOnly: true

        text: ""
        placeholderText: "Чтение данных..."
        placeholderTextColor: "#ccc"
        anchors {
            left: typeName.right
            right: btn.left
            leftMargin: 10
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
    }
    Button {
        id: btn

        width: 46
        height: 46

        display: AbstractButton.IconOnly

        font.pixelSize: 12
        icon {
            color: "transparent"
            source: "qrc:/img/copy.png"
        }

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 10
        }

        onClicked: {
            txtField.selectAll()
            txtField.copy()
            txtField.deselect()
        }
    }
}
