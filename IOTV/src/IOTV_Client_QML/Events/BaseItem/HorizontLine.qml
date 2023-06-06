import QtQuick 2.9
import QtQuick.Controls 2.5

Item{
    height: 20
    width: 400//parent.width

    Rectangle{
        height: 2
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 20
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }

        color: Qt.rgba(0, 0, 0, 0)
        border.width: 1
        border.color: "black"
        radius: 5
    }
}
