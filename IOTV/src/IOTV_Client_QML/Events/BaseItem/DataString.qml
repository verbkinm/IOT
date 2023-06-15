import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias text: dataTextField.text
    property alias label: dataText.text
//    property alias textReadOnly: dataTextField.readOnly

    height: 50
    width: 400//parent.width

    Text {
        id: dataText
        text: "Данные:"
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    TextField {
        id: dataTextField
        focus: true
        width: 200
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        font.pixelSize: 12
        text: ""
        placeholderText: "Введите данные..."
        placeholderTextColor: "#ccc"
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }
    }


//    Component.onCompleted: {
//        forceActiveFocus()
//    }

}
