import QtQuick 2.9
import QtQuick.Controls 2.5

Rectangle {

    property var _event: model.event
    property var _action: model.action

    id: componentRect
    width: parent.width * 0.8
    height: 80

    anchors.horizontalCenter: parent.horizontalCenter

    color: Qt.rgba(0, 0, 0, 0)
    radius: 5

    border.width: 1
    border.color: "green"

    Text {
        anchors.centerIn: parent
        text: model.name
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            var component = Qt.createComponent("qrc:/Events/AddEvent.qml");
            if (component.status === Component.Ready)
            {
                var object = component.createObject()

                object.title = _event.objectName
                object.evAcName = _event.objectName

                var hostNmae =  _event.host.name
                var index = object.hostNameItem.comboBox.find(hostNmae)

                if (index === -1)
                {
                    object.hostNameItem.enabled = false
                    object.hostNameItem.model = [hostNmae]
                }
                else
                {
                    object.hostNameItem.comboBox.currentIndex = index
                }

                var eventType = _event.typeString()
                index = object.eventTypeItem.comboBox.find(eventType)

                if (index === -1)
                {
                    object.eventTypeItem.enabled = false
                    object.eventTypeItem.model = [eventType]
                }
                else
                {
                    object.eventTypeItem.comboBox.currentIndex = index
                }

                console.log(_event.directionString())

                appStack.push(object)
            }
        }
    }
}
