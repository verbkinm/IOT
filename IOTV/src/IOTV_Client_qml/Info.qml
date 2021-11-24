import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
        anchors.centerIn: parent
        property int device_id: 0
        property string device_name: "None"
        property string device_description: "None description"
        property string channels: "0/0"
        title: device_name
        standardButtons: Dialog.Ok

    Label {
        text: "ID устройства: " + device_id + "<br>
                Имя устройства: \"" + device_name + "\"<br>
                Описание: \"" + device_description + "\"<br>
                Каналы R/W: " + channels + "<br>"
    }
}
