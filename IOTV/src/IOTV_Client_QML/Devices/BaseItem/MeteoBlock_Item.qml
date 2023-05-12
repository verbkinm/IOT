import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    property alias source: img.source
    property alias text: lbl.text

    signal signalClick()

    id: root

    width: parent.width * 0.8
    height: 80

    color: Qt.rgba(0, 0, 0, 0)
    border.width: 1
    border.color: Qt.rgba(0, 0, 0, 0.1)
    radius: 5

    anchors.horizontalCenter: parent.horizontalCenter

    Image {
        id: img
        fillMode: Image.PreserveAspectFit
        height: parent.height * 0.80

        anchors {
            leftMargin: 20
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
    }
    Label {
        id: lbl
        text: "-"
        font.pixelSize: 28
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignHCenter
        anchors {
            left: img.right
            leftMargin: 20
            verticalCenter: parent.verticalCenter
        }
    }

    SequentialAnimation {
        id: anim
        PropertyAnimation {
            target: root
            property: "color"
            from: Qt.rgba(0, 0, 0, 0)
            to: Qt.rgba(0, 0, 255, 0.5)
            duration: 200
        }
        PropertyAnimation  {
            target: root
            property: "color"
            from: Qt.rgba(0, 0, 255, 0.5)
            to: Qt.rgba(0, 0, 0, 0)
            duration: 200
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            anim.start()
            signalClick()
        }
    }
}
