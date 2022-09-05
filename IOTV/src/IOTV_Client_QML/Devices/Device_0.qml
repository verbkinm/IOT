import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: root

    //Ссылка на Device
    required property var device

    Column {
        id: column
        anchors.fill: parent

        Label {
            id:name
        }
        Label {
            id:devId
        }
        Label {
            id:description
        }
        Label {
            id:state
        }

        Label {
            id:rN
        }
        Label {
            id:wN
        }
    }

    Component.onCompleted: {
        title  = Qt.binding(function (){ return device.name})
        name.text = Qt.binding(function (){ return "Имя устройства: " + device.name})
        devId.text = Qt.binding(function (){ return "ID устройства: " + device.id})
        description.text = Qt.binding(function (){ return "Описание: " + device.description})
        state.text = Qt.binding(function (){ return "Состояние: " + (device.state ? "онлайн" : "офлайн")})
        rN.text = Qt.binding(function (){ return device.readChannelLength})
        wN.text = Qt.binding(function (){ return device.writeChannelLength})
    }
}
