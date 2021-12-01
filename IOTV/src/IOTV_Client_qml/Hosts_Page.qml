import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: root
    property var objectsArray: [] // объекты указатели на Device

//    width: 600
//    height: 400

    header:
        ToolBar {
            Label {
                anchors.centerIn: parent
                text: qsTr("Устройства")
                font.pixelSize: Qt.application.font.pixelSize * 2
                padding: 5
            }
        }

    Flickable{
        anchors.topMargin: 15
        anchors.fill: parent
        contentHeight: root_column.height

        Column {
            id: root_column
            width: parent.width
            spacing: 5
            padding: 5

            onChildrenChanged: {

            }

            function showInfo(sender)
            {
                var device = sender
                inf_win.device_id = device.getId()
                inf_win.device_name = device.getName()
                inf_win.device_description = device.getDescription()
                inf_win.channels = device.readChannelLength() + "/" + device.writeChannelLength()
                inf_win.visible = true
            }
        }
    }

    Info {
        id: inf_win
        visible: false
    }

    function createDivecGUI()
    {
        for (var i = 0; i < objectsArray.length; ++i)
        {
            var component
            switch(objectsArray[i].getIdToQML())
            {
                case 0:
                    component = Qt.createComponent("qrc:/Devices/Device_0.qml");
                break
                case 1:
                    component = Qt.createComponent("qrc:/Devices/Device_1.qml");
                break
                case 2:
                    component = Qt.createComponent("qrc:/Devices/Device_2.qml");
                break
                case 3:
                    component = Qt.createComponent("qrc:/Devices/Device_3.qml");
                break
                default:
                    component = Qt.createComponent("qrc:/Devices/Device_0.qml");
                break
            }

            if (component.status === Component.Ready)
            {
                var childObject = component.createObject(root_column);
                childObject.device.setDevice(objectsArray[i])
                childObject.device.connectSignals()
                childObject.name = childObject.device.getName()
                objectsArray[i].setAutoReadEnable(true)
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
