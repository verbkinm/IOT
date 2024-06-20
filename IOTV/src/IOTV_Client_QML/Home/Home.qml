import QtQuick 2.9
import QtQuick.Controls 2.5

import "qrc:/BaseItem/" as BaseItem

Page {
    id: root
    title: "Главная"

    BaseItem.GridList  {
        id: listView
        model: ListModel {id: listModel}

        Component.onCompleted: {
            updateListModel()
        }
    }

    Home_Connector {
        id: clientConnect
        text.text: "Соединение не установлено"

        Button {
            height: glob_ClientPage.btn.height
            width: glob_ClientPage.btn.width
            font.pixelSize: glob_ClientPage.btn.font.pixelSize
            text: glob_ClientPage.btn.text
            highlighted: glob_ClientPage.btn.highlighted

            anchors.top: clientConnect.text.bottom
            anchors.margins: 10
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                glob_ClientPage.btn.clicked()
            }
        }
    }

    Component.onCompleted: {
        console.log("Home page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Home page destruct: ", objectName)
    }

    Connections {
        target: client
        function onOnlineDeviceChanged()
        {
            for (var i = 0; i < listModel.count; i++)
                listModel.get(i).ledEnable = client.deviceByName(listModel.get(i).text).state
        }
        function onSignalConnected()
        {
            clientConnect.hide()
            updateListModel()
        }
        function onSignalDisconnected()
        {
            clientConnect.show()
            updateListModel()
            glob_deviceStackView.pop(homePage)
        }
        function onCountDeviceChanged()
        {
            updateListModel()
        }
    }

    function updateListModel()
    {
        listModel.clear()
        for (var i = 0; i < client.totalDevice; i++)
        {
            var objectAtributes = {
                device: client.devList()[i],
            }
            var object = {
                text: objectAtributes.device.name,
                title: objectAtributes.device.name,
                icon: imageById(objectAtributes.device.id),
                loaderSource: createDeviceBy(objectAtributes.device.id),
                ledVisible: true,
                ledEnable: objectAtributes.device.state,
                attributes: [objectAtributes],
                stackView: glob_deviceStackView
            }
            listModel.append(object)
        }
        listView.height = client.totalDevice * 100
    }

    function createDeviceBy(id) {
        switch(id)
        {
        case 1: return "/Devices/Device_1/Device_1.qml"
        case 2: return "/Devices/Device_2/Device_2.qml"
        case 4: return "/Devices/Device_4/Device_4.qml"
        case 5: return "/Devices/Device_5/Device_5.qml"
        case 8: return "/Devices/Device_8/Device_8.qml"
        }

        return "/Devices/Device_0.qml"
    }

    function imageById(id)
    {
        if (id > 0 && id <= 9)
            return "qrc:/img/id/" + id + ".png"

        return "qrc:/img/id/0.png"
    }

    function destroyDev()
    {
        updateListModel()
        //        if (loaderHome.source === "qrc:/Home/DeviceListView.qml")
        //        {
        //            loaderHome.item.destroyDev()
        //        }
    }
}
