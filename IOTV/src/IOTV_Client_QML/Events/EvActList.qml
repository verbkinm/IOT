import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/Devices/" as Devices

ListView {
    id: listView
    width: parent.width
    spacing: 15
//    interactive: false

    // Анимация появления элементов модели
    populate: Transition {
        NumberAnimation { properties: "x,y"; duration: 1000; easing.type: Easing.OutExpo }
    }
    // Анимация добавления элементов
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
        NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 300 }
        //                    NumberAnimation { properties: "x,y"; duration: 1500; easing.type: Easing.OutBounce}
    }
    // Удаление элемента
    remove: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: 400 }
        NumberAnimation { property: "scale"; from: 1.0; to: 0; duration: 300 }
    }

    model: ListModel { id: listModel }
    delegate: EvActComponent { id: evActDelegate }

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
                    action: action
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
}
