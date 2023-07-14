import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    id: root
    property alias source: img.source
    property alias text: lbl.text

    signal signalClick()

    width: parent.width
    height: 80

    anchors.horizontalCenter: parent.horizontalCenter

    Canvas {
        id: shadow
        width: parent.width
        height: parent.height
        smooth: true
        //        z: -1

        onPaint: {
            var x = rect.x
            var y = rect.y
            var r = 5
            var w = rect.width
            var h = rect.height
            var ctx = getContext("2d")
            ctx.strokeStyle = "#aaa"
            ctx.beginPath();
            ctx.moveTo(x+r, y);
            ctx.arcTo(x+w, y,   x+w, y+h, r);
            ctx.arcTo(x+w, y+h, x,   y+h, r);
            ctx.arcTo(x,   y+h, x,   y,   r);
            ctx.arcTo(x,   y,   x+w, y,   r);
            ctx.closePath();
            ctx.shadowBlur = 3;
            ctx.fill()
        }
    }

    Rectangle {
        id: rect

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

        SequentialAnimation {
            id: anim
            PropertyAnimation {
                target: rect
                property: "color"
                from: Qt.rgba(255, 255, 255, 1)
                to: Qt.rgba(0, 0, 255, 0.5)
                duration: 200
            }
            PropertyAnimation  {
                target: rect
                property: "color"
                from: Qt.rgba(0, 0, 255, 0.5)
                to: Qt.rgba(255, 255, 255, 1)
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
}
