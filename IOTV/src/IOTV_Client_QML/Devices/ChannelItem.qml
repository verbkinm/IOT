import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias textField: txtField
    property int number: 0
    property alias type: txtField.placeholderText
    property alias button: btn
    property alias buttonIcon: img.source

    height: 52

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

    TextField {
        id: txtField
        height: 52
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        font.pixelSize: 14

        text: ""
        placeholderText: "Введите данные..."
        placeholderTextColor: "#ccc"
        anchors {
            left: lb1.right
            right: btn.left
            leftMargin: 10
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
    }
    RoundButton {
        id: btn

        width: 52
        height: 52

        display: AbstractButton.IconOnly
        highlighted: true
        font.pixelSize: 12

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 10
        }
        Image {
            id: img
            anchors.centerIn: parent
            source: ""
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit // ensure it fits
        }
    }
}
