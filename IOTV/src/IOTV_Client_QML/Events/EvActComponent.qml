import QtQuick 2.9
import QtQuick.Controls 2.5

Rectangle {
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
                var object = component.createObject(null, {_event: model.event, _action: model.action})
                appStack.push(object)
            }
        }
    }
}
