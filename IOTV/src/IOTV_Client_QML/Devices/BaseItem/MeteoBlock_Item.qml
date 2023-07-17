import QtQuick 2.9
import QtQuick.Controls 2.2

ItemShadow {
    id: root
    property alias source: img.source
    property alias text: lbl.text

    signal signalClick()

    width: parent.width
    height: 80

    anchors.horizontalCenter: parent.horizontalCenter

    Rectangle {
        id: rectangle

        width: parent.width * 0.8
        height: 70

        color: Qt.rgba(255, 255, 255, 1)
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

//        SequentialAnimation {
//            id: anim
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
//        }

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
}
