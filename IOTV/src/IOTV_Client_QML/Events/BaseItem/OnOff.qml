import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    required property var obj

    id: root
    height: 50
    width: 400

    Text {
        id: hostNameText
        text: "Вкл./Выкл"

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    ComboBox {
        id: hostComboBox
        width: parent.width / 2

        model: ["Выключено", "Включено"]
        currentIndex: obj.enable ? 1 : 0

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }

        onCurrentIndexChanged: {
            obj.enable = currentIndex == 0 ? false : true
        }
    }
}
