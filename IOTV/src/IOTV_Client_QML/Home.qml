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
            text: "Состояние: " +
                  (client.state ? "подключено" : "отключено") +
                  ", кол-во устройств: (" + client.onlineDevice + "/" + client.totalDevice + ")"
            wrapMode: Text.Wrap
        }
    }

    Rectangle {
        id: controlConnect
        anchors.fill: parent
        z: 1
        opacity: 1
        visible: true

        Component.onCompleted: {
            console.log(client.state, " ", state)
        }

        states: [
            State {
                name: "hide"; when: (client != null) ? client.state : ""
                PropertyChanges { target: controlConnect; opacity: 0; visible: false }
            },
            State {
                name: "show"; when:  (client != null) ? !client.state : ""
                PropertyChanges { target: controlConnect; opacity: 1; visible: true }
            }
        ]

        transitions: [
            Transition {
                to: "hide"
                ParallelAnimation{
                    PropertyAnimation { target: controlConnect; property: "visible"; from: true; to: false; duration: 500 }
                    PropertyAnimation { target: controlConnect; property: "opacity"; from: 1; to: 0; duration: 500 }
                }
            },
            Transition {
                to: "show"
                PropertyAnimation { target: controlConnect; property: "opacity"; from: 0; to: 1; duration: 500 }
            }
        ]

        Text {
            id: txtConnection
            text: qsTr("Соединение не установлено")
            anchors.centerIn: parent
            font.pixelSize: 18
            wrapMode: Text.Wrap
        }

        Button {
            id: btnConnect
            width: 180
            font.pixelSize: 18
            text: "подключиться"

            anchors.top: txtConnection.bottom
            anchors.margins: 10
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                console.log("click")
                clientPage.connectToHost()
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
        delegate: deviceDelegate

        // Анимация появления элементов модели
        populate: Transition {
            NumberAnimation { properties: "x,y"; duration: 1000; easing.type: Easing.OutExpo }
        }
        // Анимация добавления элементов
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
            NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 100 }
            NumberAnimation { properties: "x,y"; duration: 1000; easing.type: Easing.OutBounce}
        }
        // Удаление элемента
        remove: Transition {
            PropertyAnimation{ property: "opacity"; to: 0; duration: 500}
        }
    }

    ListModel {
        id: listModel
    }

    Item {
        property string title

        id: loaderItem

        Loader {
            id: loaderDevice
            anchors.fill: parent
            source: ""
        }
    }

    Component {
        id: deviceDelegate

        Rectangle {
            id: componentRect

            states: [
                State {
                    name: "online"
                    when: client.deviceByName(model.name).state
                    PropertyChanges { target: componentRect; color: Qt.rgba(0, 1, 0, 0.1)}
                },
                State {
                    name: "offline"
                    when: !client.deviceByName(model.name).state
                    PropertyChanges { target: componentRect; color: Qt.rgba(1, 0, 0, 0.1)}
                }
            ]
            width: 120
            height: 120

            border.width: 1
            border.color: Qt.rgba(0, 0, 0, 0.5)
            radius: 5

            color: Qt.rgba(1, 0, 0, 0.1)
//            ColorAnimation on color {
//                id: animColorOffline
//                to:  Qt.rgba(1, 0, 0, 0.1)
//                duration: 1000
//                running : false
//            }
//            ColorAnimation on color {
//                id: animColorOnline
//                to: Qt.rgba(0, 1, 0, 0.1)
//                duration: 1000
//                running: false
//            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    loaderItem.objectName = loaderItem.title = client.deviceByName(model.name).aliasName
                    loaderDevice.setSource(createDeviceBy(client.deviceByName(model.name).id), {device: client.deviceByName(model.name)})
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
                text: (client != null) ? (client.deviceByName(model.name).aliasName) : ""
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
//                function onStateChanged() {
//                    if (target.state)
//                        animColorOnline.running = true
//                    else
//                        animColorOffline.running = true
//                }
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
            listModel.clear()
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

//        function onOnlineDeviceChanged()
//        {
//            onCountDeviceChanged()
//        }

        function onSignalDisconnected() {
            listModel.clear()
            loaderDevice.setSource("")
            if (appStack.currentItem != clientPage)
                appStack.pop(homePage)
        }
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
