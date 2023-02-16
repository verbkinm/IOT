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

    Home_Connector {}

    GridView {
        id: listView
        cellHeight: 130
        cellWidth: 130

        anchors {
            fill: parent
            leftMargin: parent.width * 0.1
            rightMargin: parent.width * 0.1
            topMargin: 15
        }

        model: ListModel { id: listModel }
        delegate: DeviceComponent { id: componentRect }

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

    Loader {
        property string title
        id: loaderDevice
        source: ""
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

        function onSignalDisconnected()
        {
            listModel.clear()
            loaderDevice.setSource("")

            //!!!
            if (appStack.currentItem == homePage)
                appStack.pop(homePage)
            else if (appStack.currentItem == clientPage)
            {
                appStack.pop(homePage)
                appStack.push(clientPage)
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
            return "qrc:/img/id/3.png"
        else if (id === 4)
            return "qrc:/img/id/4.png"
        else
            return "qrc:/img/id/0.png"
    }

    function loaderClear()
    {
        loaderDevice.setSource("")
    }
}
