import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: root
    property var objectsArray: [] // объекты указатели на Device

    width: 600
    height: 400

    header: Label {
        text: qsTr("Хосты")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 5
    }

    Column {
        id: root_column
        spacing: 5
        padding: 5

        function showInfo(sender)
        {
            var device = sender.device.getDevice()
            inf_win.device_id = device.getId()
            inf_win.device_name = device.getName()
            inf_win.device_description = device.getDescription()
            inf_win.channels = device.readChannelLength() + "/" + device.writeChannelLength()
            inf_win.visible = true
        }
    }

    Info {
        id: inf_win
        visible: false

        property int device_id: 0
        property string device_name: "None"
        property string device_description: "None description"
        property string channels: "0/0"

        text: "ID устройства: " + device_id + "<br>
                Имя устройства: \"" + device_name + "\"<br>
                Описание: \"" + device_description + "\"<br>
                Каналы R/W: " + channels + "<br>"

    }

    function createDivecGUI()
    {
        for (var i = 0; i < objectsArray.length; ++i)
        {
            var component = Qt.createComponent("Divice_0.qml");
            if (component.status === Component.Ready)
            {
                var childObject = component.createObject(root_column);
                childObject.width = root.width - 10
                childObject.height = 100
                childObject.name = objectsArray[i].getName()
                childObject.device.setDevice(objectsArray[i])
                childObject.device.connectSignals()
            }
        }
    }

    function clear()
    {
        var childs = root_column.children
        for (var i = 0; i < childs.length; ++i)
        {
            childs[i].destroy()
        }
    }
}
