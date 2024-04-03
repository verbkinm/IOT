import QtQuick
import QtQuick.Controls

Rectangle {
    id: notify

    width: parent.width * 0.8
    height: lbl.height + 20

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) - 20)

    color: "#e4e4e4"

    visible: opacity !== 0

    border.width: 1
    border.color: "black"
    radius: 5

    Label {
        id: lbl
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 18
        wrapMode: Text.Wrap
        text: "IOTV Client " + Qt.application.version
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

    function set_text(text)
    {
        anim.running = false
        opacity = 1
        lbl.text = text
        anim.running = true
    }
}
