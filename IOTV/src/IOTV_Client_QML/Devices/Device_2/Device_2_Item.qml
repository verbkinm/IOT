import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    property alias source: img.source
    property alias text: lbl.text


    width: parent.width * 0.8
    height: 80

    color: Qt.rgba(0, 0, 0, 0)
    border.width: 1

    Image {
        id: img
        fillMode: Image.PreserveAspectFit
        height: parent.height * 0.80

        anchors {
            rightMargin: 20
            right: parent.horizontalCenter
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
}
