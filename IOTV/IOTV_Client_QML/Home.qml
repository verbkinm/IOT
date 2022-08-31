import QtQuick 2.9
import QtQuick.Controls 2.5

Page {
    property bool status: false

    property int countDevice: 0
    property int countDeviceOnline: 0

    title: "Home"

    Flickable
    {
        id: flickable_home
        anchors.fill: parent
        contentHeight: childrenRect.height

        ScrollBar.vertical: ScrollBar { }

        Label {
            id: label1
            text: "Состояние: " + (status ? "подключён" : "отключён")
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.topMargin: 10
            anchors.top: parent.top
        }

        Label {
            text: "Количество устройств: (" + countDeviceOnline + "/" + countDevice + ")"
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.topMargin: 10
            anchors.top: label1.bottom
        }
    }
}
