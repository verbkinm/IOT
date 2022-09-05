import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: root

    //Ссылка на Device
    property var device: nullptr

    Column {
        id: column
        anchors.fill: parent

        Label {
            id:l1
        }
        Label {
            id:l2
        }
        Label {
            id:l3
        }
        Label {
            id:l4
        }
    }

    Component.onCompleted: {
        title  = Qt.binding(function (){ return device.name})
        l1.text = Qt.binding(function (){ return "Имя устройства: " + device.name})
        l2.text = Qt.binding(function (){ return "ID устройства: " + device.id})
        l3.text = Qt.binding(function (){ return "Описание: " + device.description})
        l4.text = Qt.binding(function (){ return "Состояние: " + (device.state ? "онлайн" : "офлайн")})
    }
}
