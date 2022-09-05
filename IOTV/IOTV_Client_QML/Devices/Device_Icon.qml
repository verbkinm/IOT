import QtQuick 2.0
import QtQuick.Controls 2.15
import io.qt.Backend_Device 1.0

Rectangle {
    property alias name: title.text
    property var device: nullptr
    property bool state: false

    id: root
    width: 70
    height: 70

    border.width: 1
    border.color: "red"

    color: "#00000000"
    radius: 5
    smooth: true

//    Component.onCompleted: {
//        name = device.getName()
//        state = Qt.binding(function() { return device.state })

//        var imageName = ""

        if (device.id === 0)
            imageName = "qrc:/img/info.png"
        else if (device.id === 1)
            imageName = "qrc:/img/led_off.png"
        else if (device.id === 2)
            imageName = "qrc:/img/led_on.png"

//        img.source = imageName
//    }

    onStateChanged: {
        if (state)
            border.color = "green"
        else
            border.color = "red"

        console.log("state changed")
    }

    Label {
        id: title
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: "Unknow name"
        wrapMode: Text.WordWrap
    }

    Image {
        id: img
//        source: "qrc:/img/Unknow.png"
        sourceSize: Qt.size(36, 36)
        anchors.centerIn: parent
    }
}



