import QtQuick 2.2
import QtQuick.Controls 2.2

Dialog {
    property alias text: lbl.text

    id: dialog
    modal: true

//    width: parent.width * 0.8
//    height: parent.height < 300 ? 250 : parent.height * 0.5

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    Label {
        id: lbl
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 18
        wrapMode: Text.Wrap
    }
}
