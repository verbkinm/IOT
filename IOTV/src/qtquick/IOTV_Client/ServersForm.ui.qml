import QtQuick 2.4
import QtQuick.Controls 2.15

Page {
    id: page
    width: 400
    height: 400

    title: "Servers"

    Label {
        id: label
        text: qsTr("Label")
        anchors.left: parent.horizontalCenter
        anchors.right: parent.horizontalCenter
        anchors.top: parent.verticalCenter
        anchors.bottom: parent.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.rightMargin: -200
        anchors.leftMargin: -200
        anchors.bottomMargin: -200
        anchors.topMargin: -200
    }
}
