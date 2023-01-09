import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.1

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
            wrapMode: Text.Wrap
        }
    }

    Text {
        text: qsTr("Подключения не установлено")
        anchors.centerIn: parent
        font.pixelSize: 18
        visible: !client.state
        z: 1
        wrapMode: Text.Wrap

        MouseArea {
            anchors.fill: parent
            onClicked: {
                appStack.push(clientPage)
                //                popupWait.open()
            }
        }
    }

    GridView /*ListView*/{
        id: listView
        cellHeight: 130
        cellWidth: 130

        anchors {
            fill: parent
            leftMargin: parent.width * 0.1
            rightMargin: parent.width * 0.1
            topMargin: 15
        }

        model: listModel
        delegate: contactDelegate
        //        spacing: 15
    }

    ListModel {
        id: listModel
    }

    Item {
        property string title

        id: loaderItem
//        parent: appStack

        Loader {
            id: loaderDevice
            anchors.fill: parent
            source: ""
        }
    }

    Component {
        id: contactDelegate

        Rectangle {
            id: componentRect

            width: 120
            height: 120

            border.width: 1
            border.color: Qt.rgba(0, 0, 0, 0.5)
            radius: 15

            color: Qt.rgba(1, 0, 0, 0.1)
            ColorAnimation on color {
                id: animColorOffline
                to:  Qt.rgba(1, 0, 0, 0.1)
                duration: 1000
                running : false
            }
            ColorAnimation on color {
                id: animColorOnline
                to: Qt.rgba(0, 1, 0, 0.1)
                duration: 1000
                running: false
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    loaderItem.objectName = loaderItem.title = client.deviceByName(model.name).aliasName
                    loaderDevice.setSource(createDeviceBy(client.deviceByName(name).id), {device: client.deviceByName(name)})
                    appStack.push(loaderItem)
                }
            }

            Image
            {
                id: icon
                source: model.source
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                    topMargin: 10
                }
                width: 54
                height: 54
                fillMode: Image.PreserveAspectFit
            }

            Label {
                id: devName
                text: client.deviceByName(model.name).aliasName
                font.pixelSize: 16
                anchors {
                    left: parent.left
                    right: parent.right
                    top: icon.bottom
                    bottom: parent.bottom
                    leftMargin: 10
                    rightMargin: 10
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                elide: Text.ElideRight

                Settings {
                    id: setting
                    category: model.name
                    property string name

                    Component.onCompleted: {
                        if (this.name.length === 0)
                            this.name = model.name

                        var dev = client.deviceByName(model.name)
                        dev.aliasName = this.name
                    }
                }
            }

            Connections {
                target: client.deviceByName(name)
                function onStateChanged() {
                    if (target.state)
                        animColorOnline.running = true
                    else
                        animColorOffline.running = true
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

    function createDeviceBy(id)
    {
        if (id === 1)
            return "/Devices/Device_1.qml"
        else if (id === 2)
            return "/Devices/Device_2.qml"
        else if (id === 4)
            return "/Devices/Device_4.qml"
        else
            return "/Devices/Device_0.qml"
    }

    function imageById(id)
    {
        if (id === 1)
            return "qrc:/img/id/1.png"
        else if (id === 2)
            return "qrc:/img/id/2.png"
        else if (id === 3)
            return "qrc:/img/id/3.png"
        else if (id === 4)
            return "qrc:/img/id/4.png"
        else
            return "qrc:/img/id/0.png"
    }
}
