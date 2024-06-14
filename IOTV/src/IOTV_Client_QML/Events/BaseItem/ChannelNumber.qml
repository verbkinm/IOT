import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias label: chNumberText.text
    property alias value: chNumberSpinBox.value

    signal signalNumberChanged();

    height: 50
    width: 400

    Text {
        id: chNumberText
        text: "№ канала:"
        width: parent.width / 2
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    SpinBox {
        id: chNumberSpinBox
        width: parent.width / 2
        from: 0
        to: 255

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }

        onValueChanged: {
            signalNumberChanged()
        }
    }
}
