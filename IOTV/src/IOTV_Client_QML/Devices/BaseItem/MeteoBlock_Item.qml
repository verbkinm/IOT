import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Effects

Item {
    id: root
    property alias source: img.source
    property alias text: lbl.text

    signal signalClick()

    height: 80

    anchors {
        left: parent.left
        right: parent.right
        leftMargin: parent.width - parent.width * 0.9
        rightMargin: parent.width - parent.width * 0.9
    }

    Rectangle {
        id: rectangle
        anchors.fill: parent

        radius: 5

        anchors.centerIn: parent

        Image {
            id: img
            fillMode: Image.PreserveAspectFit
            height: parent.height * 0.80

            anchors{
                left: parent.left
                leftMargin: 30
                verticalCenter: parent.verticalCenter
            }
        }

        Label {
            id: lbl
            text: "-"
            font.pixelSize: 28

            wrapMode: Text.WordWrap
            elide: Text.ElideRight

            maximumLineCount: 2

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            antialiasing: true

            anchors{
                right: parent.right
                rightMargin: 10
                left: img.right
                verticalCenter: parent.verticalCenter
            }
        }

        ParallelAnimation {
            id: animPressed
            PropertyAnimation {
                target: rectangle
                property: "color"
                from: Qt.rgba(255, 255, 255, 1)
                to: Qt.rgba(0, 0, 100, 0.5)
                duration: 200
            }
            PropertyAnimation {
                target: lbl
                property: "color"
                from: Qt.rgba(0, 0, 0, 1)
                to: Qt.rgba(255, 255, 255, 1)
                duration: 200
            }
        }
        ParallelAnimation {
            id: animReleased
            PropertyAnimation  {
                target: rectangle
                property: "color"
                from: Qt.rgba(0, 0, 100, 0.5)
                to: Qt.rgba(255, 255, 255, 1.0)
                duration: 200
            }
            PropertyAnimation {
                target: lbl
                property: "color"
                from: Qt.rgba(255, 255, 255, 1.0)
                to: Qt.rgba(0, 0, 0, 1.0)
                duration: 200
            }
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                animReleased.start()
                signalClick()
            }
            onPressed: {
                animPressed.start()
            }
            onCanceled: {
                animReleased.start()
            }
            onExited: {
                animReleased.start()
            }
        }
    }

    MultiEffect {
        id: shadowEff
        source: rectangle
        anchors.fill: rectangle
        shadowEnabled: true
        shadowOpacity: 0.7
    }
}
