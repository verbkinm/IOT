import QtQuick 2.9
import QtQuick.Controls 2.5
import "qrc:/BaseItem/" as BaseItem

GridView {
    id: listView
    cellWidth: global_window.width / 2 - global_window.width * 0.1
    cellHeight: 110

    anchors {
        fill: parent
        leftMargin: parent.width * 0.1
        rightMargin: parent.width * 0.1
        topMargin: 15
    }

    // Для загрузки единичного экземпляра устройства
    Loader {
        property string title
        id: loaderDevice
    }

    model: ListModel { id: listModel }
    delegate: DeviceComponent {
        id: componentRect

        onSignalClick: {
            loaderDevice.setSource(createDeviceBy(client.deviceByName(model.name).id), {
                                       "device": client.deviceByName(model.name)
                                   })
            loaderDevice.title = loaderDevice.objectName = Qt.binding(
                        function () {
                            return client.deviceByName(model.name).aliasName
                        })
            glob_deviceStackView.push(loaderDevice)
        }

        Connections {
            target: client.deviceByName(name)
            function onSignalUpdate() {
                model.source = imageById(target.id)
            }
        }
    }

    // Анимация появления элементов модели
    populate: Transition {
        NumberAnimation { properties: "x,y"; duration: 1000; easing.type: Easing.OutExpo }
    }
    // Анимация добавления элементов
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 500 }
        NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 500; alwaysRunToEnd: true }
    }
    // Удаление элемента
    remove: Transition {
        PropertyAnimation{ property: "opacity"; to: 0; duration: 500}
        PropertyAnimation{ property: "scale"; to: 0; duration: 500; alwaysRunToEnd: true}
    }

    Component.onCompleted: {
        updateList()
    }

    Connections {
        target: client
        function onCountDeviceChanged()
        {
            updateList()
        }
    }

    function updateList()
    {
        listModel.clear()
        for( var i = 0; i < client.totalDevice; i++)
        {
            var device = client.devList()[i];
            var object = {
                name: device.name,
                source: imageById(device.id)
            }
            listModel.append(object)
        }
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

    function destroyDev()
    {
        loaderDevice.setSource("")
    }
}
