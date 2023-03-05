import QtQuick 2.9
import QtQuick.Controls 2.5

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
        NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 300 }
//        NumberAnimation { properties: "x,y"; duration: 1000; easing.type: Easing.OutBounce}
    }
    // Удаление элемента
    remove: Transition {
        PropertyAnimation{ property: "opacity"; to: 0; duration: 500}
    }

    Component.onCompleted: {
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
    }
}