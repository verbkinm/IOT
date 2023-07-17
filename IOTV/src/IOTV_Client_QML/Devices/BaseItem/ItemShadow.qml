import QtQuick 2.9

//
Item {
    id: root
    width: 200
    height: 200

    Rectangle {
        height: parent.height
        width: parent.width

        color: Qt.rgba(0, 0, 0, 0.0)
        radius: 5

        anchors {
            centerIn: parent
        }

        Canvas {
            id: shadow
            width: parent.width
            height: parent.height
            smooth: true

            onPaint: {
                var x = rectangle.x
                var y = rectangle.y
                var r = 5
                var w = rectangle.width
                var h = rectangle.height
                var ctx = getContext("2d")
                ctx.strokeStyle = "#aaa"
                ctx.beginPath()
                ctx.moveTo(x + r, y)
                ctx.arcTo(x + w, y, x + w, y + h, r)
                ctx.arcTo(x + w, y + h, x, y + h, r)
                ctx.arcTo(x, y + h, x, y, r)
                ctx.arcTo(x, y, x + w, y, r)
                ctx.closePath()
                ctx.shadowBlur = 3
                ctx.fill()
            }
        }
    }
}
