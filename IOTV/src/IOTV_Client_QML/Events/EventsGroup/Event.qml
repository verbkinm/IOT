import QtQuick 2.9
import QtQuick.Controls 2.5

import "qrc:/Events/BaseItem" as BaseItem

Page {
    required property var _event

    property alias btnDeleteVisible: deleteEvent.visible

    id: root
    title: _event.name

    Flickable {
        width: parent.width
        height: parent.height

        contentHeight: column.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            anchors.top: parent.top
            anchors.topMargin: 15
            spacing: 15

            BaseItem.DataString {
                id: group
                width: parent.width
                label: "Группа: "
                text: (_event === null) ? "По умолчанию" : _event.groupName
                placeholderText: "Название группы"
            }

            BaseItem.DataString {
                id: name
                width: parent.width
                label: "Имя событие: "
                text: title
                placeholderText: "Название события"
            }

            BaseItem.HostNameComboBox {
                id: hostNameItem
                height: 50
                width: parent.width
                event: _event
            }

            BaseItem.EventType {
                id: eventTypeItem
                height: 50
                width: parent.width
                event: _event

                onSignalActivated: {
                    client.deleteObject(_event);
                    _event = client.createEmptyEvent(comboBox.model[comboBox.currentIndex], _event.name, _event.groupName)
                    print(_event.type)

                    eventTypeCreate(eventType, model)
                }

                Component.onCompleted: {
                    print("BaseItem.EventType complete")
                    eventTypeCreate(eventType, model)
                }
            }

            // Событие

            Loader {
                id: eventTypeLoader
                source: ""
                width: parent.width
            }

            BaseItem.HorizontLine {
                height: 20
                width: parent.width
            }

            BaseItem.HorizontLine {
                height: 20
                width: parent.width
            }

            Item {
                width: parent.width
                height: 70

                RoundButton {
                    id: deleteEvent
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        right: save.left
                        rightMargin: 20
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/delete_white.png"
                        height: 24
                        width: 24
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        client.removeEventAction(title)
                        glob_eventStackView.pop(eventsPage)
                    }
                }

                RoundButton {
                    id: save
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        right: parent.right
                        rightMargin: 20
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/save_white.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        if (name.text === "")
                        {
                            glob_dialogShared.defaultAcceptedMessage()
                            glob_dialogShared.title = "Внимание"
                            glob_dialogShared.text = "Имя события не может быть пустым!"
                            glob_dialogShared.open()
                            return
                        }

                        client.saveEvent(_event)
                        client.queryEventAction()

                        glob_eventStackView.pop(eventsPage)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Add Events page construct: ")
//        focusChanged(true)
    }

    Component.onDestruction: {
        console.log("Add Events page destruct: ", objectName)
    }

    onFocusChanged: {
//        eventTypeItem.signalActivated()
////        actionTypeItem.signalActivated()

//        focus = true
    }

    function eventTypeCreate(eventType, model)
    {
        if (eventType === model[0] || eventType === model[5] || eventType === model[6])
            hostNameItem.visible = false
        else
            hostNameItem.visible = true

        if (eventType === model[0] || eventType === model[1] || eventType === model[2])
            eventTypeLoader.setSource("qrc:/Events/BaseItem/EmptyItem.qml")
        else if(eventType === model[3])
            eventTypeLoader.setSource("qrc:/Events/BaseItem/StateType.qml", {event: _event, width: parent.width})
        else if(eventType === model[4])
            eventTypeLoader.setSource("qrc:/Events/BaseItem/DataType.qml", {event: _event, width: parent.width})
        else if(eventType === model[5])
            eventTypeLoader.setSource("qrc:/Events/BaseItem/AlarmType.qml", {event: _event, width: parent.width})
        else if(eventType === model[6])
            eventTypeLoader.setSource("qrc:/Events/BaseItem/TimerType.qml", {event: _event, width: parent.width})
    }
}
