import QtQuick 2.9
import QtQuick.Controls 2.5

Column {
    property alias srcHostName: srcHost.startHostName
    property alias dstHostName: dstHost.startHostName
    property alias srcChannelNumber: srcChNum.value
    property alias dstChannelNumber: dstChNum.value

    width: 400//parent.width
    spacing: 15

    HostNameComboBox {
        id: srcHost
        height: 50
        width: parent.width
        label: "Источник"
    }

    ChannelNumber {
        id: srcChNum
        label: "Канал источника"
        width: parent.width
    }

    HostNameComboBox {
        id: dstHost
        height: 50
        width: parent.width
        label: "Приёмник"
    }

    ChannelNumber {
        id: dstChNum
        label: "Канал приёмника"
        width: parent.width
    }

    function listName() {
        var arr = []
        for( var i = 0; i < client.devList().length; i++)
            arr.push(client.devList()[i].aliasName)

        return arr
    }

    function srcHostNameText() {
        return srcHost.comboBox.currentText
    }
    function dstHostNameText() {
        return dstHost.comboBox.currentText
    }

//    Component.onCompleted: {
//        console.log(srcHostName)
//        console.log(dstHostName)
//    }
}
