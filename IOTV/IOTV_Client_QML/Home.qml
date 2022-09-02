import QtQuick 2.9
import QtQuick.Controls 2.5

Page {
    id: root
    title: "Home"
    anchors.fill: parent

    footer: Item {
        height: 20
        width: parent.width
        id: footerItem

        Text {
            anchors.left: parent.left
            anchors.margins: 5
            font.pixelSize: 12
            text: "Состояние: " + (client.state ? "подключено" : "отключено") +
                  ", кол-во устройств: (" + client.onlineDevice + "/" + client.totalDevice + ")"
        }
    }
    Connections {
        target: client
        function onCountDeviceChanged()
        {
            for( var i = 0; i < target.totalDevice; i++)
            {
                var device = target.devList()[i];
                var object = {
                    name: device.getName(),
                    source: imageById(device.id())
                }
                listModel.append(object)
            }
        }
        function onSignalDisconnected() {listModel.clear()}
    }

    GridView {
        anchors.fill: parent
        anchors.margins: 5
        cellHeight: 110
        cellWidth: 110

        model: listModel
        delegate: contactDelegate
    }

    Component {
        id: contactDelegate
        Rectangle {
            id: componentRect
            width: 100
            height: 100
            color: "lightsteelblue"
            border.width: 2
            border.color: "red"
            radius: 5
            smooth: true

            Column {
                anchors.fill: parent
                Text {
                    text: name;
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Image
                {
                    source: model.source
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Connections {
                    target: client.deviceByName(name)
                    function onStateChanged() {
                        componentRect.border.color = target.state ? "green" : "red"
                    }
                    function onSignalUpdate() {
                        model.source = imageById(target.id())
                    }
                }
            }
        }
    }

    ListModel {
        id: listModel
    }

    function imageById(id)
    {
        if (id === 1)
            return "qrc:/img/led_on.png"
        else if (id === 2)
            return "qrc:/img/led_off.png"
        else if (id === 3)
            return "qrc:/img/info.png"
        else
            return "qrc:/img/Unknow.png"
    }
}
