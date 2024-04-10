import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    id: legend
    color: "#00000000"
    height: legendRow.height
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
        Item {
            id: rect
            property var currentSeries: seriesNames[index]
            property var currentName: labelNames[index]
            property color markerColor: seriesColors[index]

            //            border {
            //                color: "black"
            //                width: 1
            //            }

//            color: "#00000000"
            //            radius: 4
            //            width: {legend.width / seriesCount - legendRow.spacing * legendRow}
            implicitWidth: marker.width + label.width// legend.width / seriesCount - legendRow.spacing * seriesCount
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
                anchors {
                    leftMargin: 50
                    rightMargin: 15
                }
            }
            Text {
                id: label
                width: implicitWidth
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: marker.right
                    leftMargin: 5
                }
                text: currentName
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 8

                elide: Text.ElideRight

                anchors {
                    leftMargin: 15
                    rightMargin: 15
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    legend.selected(currentSeries);
                    label.font.strikeout = !label.font.strikeout
                }
            }
        }
    }

    Flow {
        id: legendRow
        spacing: 10
        leftPadding: 15
        rightPadding: 15
        width: parent.width

        Repeater {
            id: legendRepeater
            model: seriesCount
            delegate: legendDelegate
        }

        function mar(){
            var rowCount = parent.width / (legendRow.children[0].width + legendRow.spacing);
            if(rowCount> legendRow.children.length)
                rowCount = legendRow.children.length

            rowCount = parseInt(rowCount)
            var rowWidth = rowCount * legendRow.children[0].width + (rowCount - 1) * legendRow.spacing
//            print(legendRow.height)
            return (parent.width - rowWidth) / 2
        }
    }
}

