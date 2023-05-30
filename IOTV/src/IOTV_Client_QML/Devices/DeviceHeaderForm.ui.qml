import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    height: 64
    width: 400
    color: "#ffffff"
    border.color: "#ffffff"
    property alias stateDevText: stateDev.text

    Text {
        id: stateDev
        color: "grey"
        text: "unknow"
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: settings.left
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        layer.smooth: true
    }

    Button {
        id: settings

        width: 52
        height: 52
        wheelEnabled: false
        autoExclusive: false
        checkable: false
        flat: false
        antialiasing: true

        anchors {
            right: debugMode.left
            verticalCenter: parent.verticalCenter
            rightMargin: 10
        }
        display: AbstractButton.IconOnly
        icon {
            color: "transparent"
            source: "qrc:/img/settings.png"
        }
    }

    Button {
        id: debugMode

        width: 52
        height: 52

        anchors {
            right: info.left
            verticalCenter: parent.verticalCenter
            rightMargin: 10
        }
        display: AbstractButton.IconOnly
        icon {
            color: "transparent"
            source: "qrc:/img/debug.png"
        }
    }

    Button {
        id: info

        width: 52
        height: 52

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: 10
        }
        display: AbstractButton.IconOnly
        icon {
            color: "transparent"
            source: "qrc:/img/info.png"
        }
    }

    Loader {
        property string title: device.aliasName

        id: loaderDebug
        objectName: "debug"
        source: ""
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:1.25}D{i:1}D{i:2}D{i:3}D{i:4}D{i:5}
}
##^##*/

