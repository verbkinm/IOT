import QtQuick 2.15
import QtQuick.Shapes
import QtQuick.Controls 2.15

Rectangle{
    id: root
    layer.enabled: true
    layer.samples: 8
    color: "#00000000"

    property int radius1: (width < height) ? root.width / 2 : root.height / 2
    property int radius2: radius1 - (radius1 / 50 * 15)
    property real centerX: width / 2
    property real centerY: height / 2
    property real minValue: 0
    property real maxValue: 100
    property real minMiddle: 20
    property real maxMiddle: 60
    property real value: 5
    property string postfix: ""

    Shape {
        id: sh
        smooth: true
        antialiasing: true
        anchors.fill: parent

        // Первый цвет
        ShapePath {
            fillColor: "blue"
            strokeWidth: 0
            PathAngleArc {
                id: p1
                centerX: root.centerX
                centerY: root.centerY
                radiusX: radius1
                radiusY: radius1
                startAngle: 135
                sweepAngle: rangeToRange(minValue, maxValue, 135, 405, minMiddle)
            }
            PathLine {
                x: centerX
                y: centerY
            }
        }
        // Второй цвет
        ShapePath {
            fillColor: "green"
            strokeWidth: 0
            PathAngleArc {
                id: p2
                centerX: root.centerX
                centerY: root.centerY
                radiusX: radius1
                radiusY: radius1
                startAngle: p1.startAngle + p1.sweepAngle// - 1
                sweepAngle: rangeToRange(minValue, maxValue, 135, 405, maxMiddle) - p1.sweepAngle
            }
            PathLine {
                x: centerX
                y: centerY
            }
        }
//        // Третий цвет
        ShapePath {
            fillColor: "red"
            strokeWidth: 0
            PathAngleArc {
                centerX: root.centerX
                centerY: root.centerY
                radiusX: radius1
                radiusY: radius1
                startAngle: p2.startAngle + p2.sweepAngle// - 1
                sweepAngle: rangeToRange(minValue, maxValue, 135, 405, maxValue) - p1.sweepAngle - p2.sweepAngle
            }
            PathLine {
                x: centerX
                y: centerY
            }
        }
        // Белая середина
        ShapePath {
            fillColor: "white"
            strokeWidth: 0
            PathAngleArc {
                centerX: root.centerX
                centerY: root.centerY
                radiusX: radius2
                radiusY: radius2
                startAngle: 45
                sweepAngle: -270
            }
        }
        // Внешняя арка
        ShapePath {
            id: sp
            fillColor: "transparent"
            strokeColor: "black"
            strokeStyle: ShapePath.SolidLine
            strokeWidth: 2
            startX: 0
            startY: 0
            PathAngleArc {
                id: arc1
                centerX: root.centerX
                centerY: root.centerY
                radiusX: radius1 - sp.strokeWidth
                radiusY: radius1 - sp.strokeWidth
                startAngle: 45
                sweepAngle: -270
            }
        }
        // Внутренняя арка
        ShapePath {
            id: sp2
            fillColor: "transparent"
            strokeColor: "black"
            strokeStyle: ShapePath.SolidLine
            strokeWidth: 2
            PathAngleArc {
                id: arc2
                centerX: root.centerX
                centerY: root.centerY
                radiusX: radius2
                radiusY: radius2
                startAngle: 45
                sweepAngle: -270
            }
        }
        // Стрелка
        ShapePath {
            id: tr
            property int angle: 0
            property int width: 1
            fillColor: "orange"
            strokeColor: "black"
            strokeStyle: ShapePath.SolidLine
            strokeWidth: width
            startX: centerX + Math.cos((135 + rangeToRange(minValue, maxValue, 135, 405, value)) * 3.14 / 180) * (radius2 + radius2 * 0.2)
            startY: centerY + Math.sin((135 + rangeToRange(minValue, maxValue, 135, 405, value)) * 3.14 / 180) * (radius2 + radius2 * 0.2)
            PathLine {
                id: f_l
                x: tr.startX + Math.cos((17 + 135 + rangeToRange(minValue, maxValue, 135, 405, value)) * 3.14 / 180) * (radius2 - radius1 * 1.05)
                y: tr.startY + Math.sin((17 + 135 + rangeToRange(minValue, maxValue, 135, 405, value)) * 3.14 / 180) * (radius2 - radius1 * 1.05)
            }
            PathLine {
                x: tr.startX
                y: tr.startY
            }
            PathLine {
                x: tr.startX + Math.cos((-17 + 135 + rangeToRange(minValue, maxValue, 135, 405, value)) * 3.14 / 180) * (radius2 - radius1 * 1.05)
                y: tr.startY + Math.sin((-17 + 135 + rangeToRange(minValue, maxValue, 135, 405, value)) * 3.14 / 180) * (radius2 - radius1 * 1.05)
            }
            PathLine {
                x: f_l.x
                y: f_l.y
            }
        }
    }

    Label {
        anchors.centerIn: parent
        text: value + "<br>" + postfix
        font.pixelSize: radius2 * 0.5
        horizontalAlignment: Text.AlignHCenter
    }
//    Label {
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.bottom: parent.bottom
//        text: postfix
//        font.pixelSize: radius2 * 0.5
//    }

    function rangeToRange(min1, max1, min2, max2, value)
    {
        return min2 + (max2 - min2) * ((value - min1) / (max1 - min1)) - min2
    }
}
