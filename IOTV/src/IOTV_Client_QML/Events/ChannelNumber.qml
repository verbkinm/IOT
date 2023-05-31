import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias label: chNumberText.text
    property alias value: chNumberSpinBox.value

    height: 50
    width: 400//parent.width

    Text {
        id: chNumberText
        text: "№ канала:"
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    SpinBox {
        id: chNumberSpinBox
        width: 200
        from: 0
        to: 255

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }
    }
}
