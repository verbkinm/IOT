import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Page {
    id: root
    title: "Главная"

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

    GridView {
        id: listView

        anchors.fill: parent
        anchors.margins: 10

        cellHeight: 170
        cellWidth: 170

        model: listModel
        delegate: contactDelegate
    }

    ListModel {
        id: listModel
    }

    Component {
        id: contactDelegate

        Rectangle {
            id: componentRect

            width: 164
            height: 164

            color: "lightsteelblue"
            border.width: 2
            border.color: "red"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var find = false
                    var pageObject
                    for (var i = 0; i < appStack.children.length; i++)
                        if (appStack.children[i].objectName === name)
                        {
                            find = true
                            pageObject = appStack.children[i]
                            break
                        }
                    if (find)
                    {
                        appStack.push(pageObject)
                        return
                    }

                    var component = Qt.createComponent(createDeviceBy(client.deviceByName(name).id));
                    if (component.status === Component.Ready)
                    {
                        var dev = client.deviceByName(name)
                        var obj = component.createObject(appStack, {device: dev})
                        obj.objectName = name
                        appStack.push(obj);
                        dev.signalUpdate.connect(function() {pressBack.clicked()})
                    }
                }

                function createDeviceBy(id)
                {
                    if (id === 1)
                        return "/Devices/Device_1.qml"
                    else if (id === 4)
                        return "/Devices/Device_4.qml"
                    else
                        return "/Devices/Device_0.qml"
                }
            }

            Text {
                id: devName
                text: model.name;
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.margins: 5
            }

            Image
            {
                source: model.source
                anchors.top: devName.bottom
                anchors.topMargin: 5
                anchors.horizontalCenter: parent.horizontalCenter
                width: 124
                height: 124
                fillMode: Image.PreserveAspectFit
            }


            Connections {
                target: client.deviceByName(name)
                function onStateChanged() {
                    componentRect.border.color = target.state ? "green" : "red"
                }
                function onSignalUpdate() {
                    model.source = imageById(target.id)
                }
            }
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
                    name: device.name,
                    source: imageById(device.id)
                }
                listModel.append(object)
            }
        }
        function onSignalDisconnected() {listModel.clear()}
    }

    function imageById(id)
    {
        if (id === 1)
            return "qrc:/img/id/1.png"
        else if (id === 2)
            return "qrc:/img/id/2.png"
        else if (id === 3)
            return "qrc:/img/info.png"
        else if (id === 4)
            return "qrc:/img/id/4.png"
        else
            return "qrc:/img/Unknow.png"
    }
}
