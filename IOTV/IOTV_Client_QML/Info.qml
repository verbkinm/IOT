import QtQuick 2.0

Rectangle {
    property alias text: txt.text
    opacity: 0.9

    anchors.fill: parent
    Rectangle{
        color: "white"
        border.width: 1
        border.color: "black"
        radius: 5
        height: parent.height * 0.6
        width: parent.width * 0.5
        anchors.centerIn: parent

        Text {
            id: txt
            anchors.centerIn: parent
            text: qsTr("Info")
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            parent.visible = false
        }
    }
}
