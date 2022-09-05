import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Page {
    id: root
    title: "Home"
//    anchors.fill: parent

    signal openDevice(string name)

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
                    name: device.name,
                    source: imageById(device.id)
                }
                listModel.append(object)
            }
        }
        function onSignalDisconnected() {listModel.clear()}
    }

    Rectangle {
        id:re
        anchors{
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        //    width: 300
        anchors.leftMargin: 30
        anchors.rightMargin: 30
        border.color: "red"
        border.width: 1
    }
    GridView {
        anchors.fill: parent
        anchors.margins: 5
        cellHeight: 110
        cellWidth: 110

        model: listModel
        delegate: contactDelegate
    }

    onWidthChanged: {
        console.log("gridView width = " + gridView.width)
        console.log("gridView parent width = " + gridView.parent.width)
        console.log("re width = " + re.width)
//        console.log("gridView childrenRect width = " + gridView.)
    }

    Component.onCompleted: {
        console.log("gridView width = " + gridView.width)
        console.log("gridView parent width = " + gridView.parent.width)
        console.log("re width = " + re.width)
        console.log("gridView childrenRect width = " + gridView.childrenRect.width)
    }



    ListModel {
        id: listModel

        ListElement {}
        ListElement {}
        ListElement {}
        ListElement {}
    }

    Component {
        id: contactDelegate
        Rectangle {
            id: componentRect
            width: 100
            height: 100
            color: "lightsteelblue"
            border.width: 0
            border.color: "red"
            radius: 5
            smooth: true

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    openDevice(name)
                }
            }

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
                    width: 64
                    height: 64
                    //                    fillMode: Image.PreserveAspectFit
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
    }

    function imageById(id)
    {
        if (id === 1)
            return "qrc:/img/id/1.png"
        else if (id === 2)
            return "qrc:/img/id/2.png"
        else if (id === 3)
            return "qrc:/img/info.png"
        else
            return "qrc:/img/Unknow.png"
    }
}
