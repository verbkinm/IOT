import QtQuick 2.2
import QtQuick.Controls 2.2

Rectangle {
    property alias text: lbl.text

    id: notify
    width: parent.width * 0.8
    height: lbl.height + 20

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) - 20)

    color: "#e4e4e4"
    z:1

    visible: opacity != 0

    border.width: 1
    border.color: "black"
    radius: 3

    Label {
        id: lbl
        anchors.centerIn: parent
        font.pixelSize: 18
        wrapMode: Text.Wrap
        text: "Example text"
    }

    NumberAnimation on opacity {
        id: anim
        easing.type: Easing.InQuad
        to: 0
        running: true
        duration: 2000

    }

    MouseArea {
        anchors.fill: parent

        onPressed: {
            anim.running = false
            notify.opacity = 1
        }
        onReleased: anim.running = true
    }

    function open()
    {
        console.log("open")
        opacity = 1
    }
}
