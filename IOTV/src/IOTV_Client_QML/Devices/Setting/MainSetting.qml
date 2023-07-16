import QtQuick 2.9
import QtQuick.Controls 2.2
import Qt.labs.settings 1.1

Rectangle {
    required property string deviceName

    id: root

    width: parent.width
    height: 40

    color: Qt.rgba(0, 0, 0, 0)

    //    Label {
    //        id: lbl
    //        wrapMode: Text.Wrap
    //        text: "Имя устройства: "
    //        font.pixelSize: 14
    //        elide: Text.ElideRight

    //        anchors {
    //            left: parent.left
    //            verticalCenter: parent.verticalCenter
    //            leftMargin: 15
    //            rightMargin: 15
    //        }
    //    }
    TextField {
        id: txt
        height: 52
        antialiasing: true
        font.pixelSize: 14
        horizontalAlignment: Text.AlignHCenter
        placeholderText: "Имя устройства"

        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            leftMargin: 15
            rightMargin: 15
        }
    }

    Settings {
        id: settings
        category: deviceName
        property alias name: txt.text

        onNameChanged: {
            client.deviceByName(deviceName).aliasName = this.name
            sync()
        }
    }
}
