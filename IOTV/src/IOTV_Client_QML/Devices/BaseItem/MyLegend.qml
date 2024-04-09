import QtQuick
import QtQuick.Layouts

Rectangle {
    id: legend
    color: "#00000000"
    anchors {
        topMargin: 10
        bottomMargin: 0
    }

    //    z:15
    property int seriesCount: 0
    property var seriesNames: []
    property var seriesColors: []
    property var labelNames: []
    signal selected(var series)

    function addSeries(seriesName,labelName, color) {
        seriesNames.push(seriesName)
        labelNames.push(labelName)
        seriesColors.push(color)
        seriesCount++;
    }

    function removeSeries(seriesName){
        var s = seriesNames.indexOf(seriesName)
        seriesNames.splice(s,1)
        unitNames.splice(s,1)
        seriesColors.splice(s,1)
        seriesCount--;
    }

    Component {
        id: legendDelegate
        Rectangle {
            id: rect
            property var currentSeries: seriesNames[index]
            property var currentName: labelNames[index]
            property color markerColor: seriesColors[index]

            border {
                color: "black"
                width: 1
            }

//            color: "#00000000"
//            radius: 4
//            width: {legend.width / seriesCount - legendRow.spacing * legendRow}
            width: marker.width + label.width// legend.width / seriesCount - legendRow.spacing * seriesCount
            implicitHeight: label.implicitHeight + marker.implicitHeight + 10

            Rectangle {
                id: marker
                anchors.verticalCenter: parent.verticalCenter
                color: markerColor
                width: 10
                height: width
                radius: 15
                border {
                    color: "black"
                    width: 1
                }
            }
            Text {
                id: label
//                width: parent.width - marker.width
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: marker.right
                    leftMargin: 5
                }
                text: currentName
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                font.pointSize: 8
                elide: Text.ElideRight
                 Layout.fillWidth: true
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    emit: legend.selected(currentSeries);
                }
            }
        }
    }

    Flow {
        id: legendRow
        spacing: 15
        anchors.centerIn: parent
//        anchors.fill: parent

        Repeater {
            id: legendRepeater
            model: seriesCount
            delegate: legendDelegate
        }

    }
}

