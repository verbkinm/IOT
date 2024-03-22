import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/Devices/" as Devices

GridView {
    id: listView
    cellWidth: global_window.width / 2 - global_window.width * 0.1
    cellHeight: 110

    // Для загрузки единичного экземпляра устройства
    Loader {
        property string title
        id: acEvLoader
    }

    model: ListModel { id: listModel }
    delegate: EvActComponent {
        id: evActDelegate

        onSignalClick: {
            acEvLoader.setSource("qrc:/Events/AddEvent.qml",
                                 {_event: model.event, _action: model.action, title: model.name})
            acEvLoader.title = model.name
            glob_eventStackView.push(acEvLoader)
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
        listModel.clear()
    }

    Connections {
        target: client
        function onSignalEventAction() {
            timer.stop()

            listModel.clear()

            var list = target.evAcList()

            for( var i = 0; i < list.length; i++)
            {
                var event = list[i][0]
                var action = list[i][1]

                var object = {
                    name: event["name"],
                    event: event,
                    action: action,
                    source: actionImageByEventType(event["type"])
                }

                listModel.append(object)
            }
            listView.height = list.length * 100
        }
        function onSignalDisconnected() {
            glob_eventStackView.pop(eventsPage)
            timer.start()
            listModel.clear()
        }
        function onCountDeviceChanged()
        {
            glob_eventStackView.pop(eventsPage)
            glob_eventStackView.pop(eventsPage)
            timer.start()
            listModel.clear()
        }
    }

    function actionImageByEventType(eventType)
    {
        switch (eventType)
        {
        case "connection":
            return "qrc:/img/events/connected.png"
        case "disconnection":
            return "qrc:/img/events/disconnected.png"
        case "state":
            return "qrc:/img/id/0.png"
        case "data":
            return "qrc:/img/events/data.png"
        case "alarm":
            return "qrc:/img/events/alarm.png"
        case "timer":
            return "qrc:/img/events/timer.png"
        }

        return "qrc:/img/id/0.png"
    }

    function destroyEv()
    {
        acEvLoader.setSource("")
    }
}
