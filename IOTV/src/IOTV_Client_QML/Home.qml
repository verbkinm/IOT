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

            //            anchors.margins: 5

            //                ScaleAnimator {
            //                    id: scaleAnimMin
            //                    target: componentRect
            //                    from: 1
            //                    to: 0.85
            //                    easing.type: Easing.InCubic;
            //                    duration: 150
            //                    running: false
            //                }
            //                ScaleAnimator {
            //                    id: scaleAnimMax
            //                    target: componentRect
            //                    from: 0.85
            //                    to: 1
            //                    easing.type: Easing.InCubic;
            //                    duration: 150
            //                    running: false
            //                }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    //                    var find = false
                    //                    var pageObject
                    //                    for (var i = 0; i < appStack.children.length; i++)
                    //                        if (appStack.children[i].objectName === name)
                    //                        {
                    //                            find = true
                    //                            pageObject = appStack.children[i]
                    //                            break
                    //                        }
                    //                    if (find)
                    //                    {
                    //                        appStack.push(pageObject)
                    //                        return
                    //                    }

                    var component = Qt.createComponent(createDeviceBy(client.deviceByName(name).id));
                    if (component.status === Component.Ready)
                    {
                        var dev = client.deviceByName(name)
                        var obj = component.createObject(appStack, {device: dev})
                        obj.objectName = name
                        appStack.push(obj);
                        dev.signalUpdate.connect(function() {pressBack.clicked()})
                    }
                    //                        scaleAnimMin.running = true
                    //                        scaleAnimMax.running = true
                }

                //                    onEntered: {
                //                        scaleAnimMin.running = true
                //                    }
                //                    onExited: {
                //                        scaleAnimMax.running = true
                //                    }

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
                text: model.name;
                font.pixelSize: 16
                anchors {
                    left: parent.left
                    right: parent.right
                    top: icon.bottom
                    bottom: parent.bottom
                    leftMargin: 5
                    rightMargin: 5
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                elide: Text.ElideRight
            }

            Connections {
                target: client.deviceByName(name)
                function onStateChanged() {
                    if (target.state)
                        animColorOnline.running = true
                    else
                        animColorOffline.running = true

                    //                        componentRect.color = target.state ? Qt.rgba(0, 1, 0, 0.1) : Qt.rgba(1, 0, 0, 0.1)
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
            return "qrc:/img/id/3.png"
        else if (id === 4)
            return "qrc:/img/id/4.png"
        else
            return "qrc:/img/id/0.png"
    }
}
