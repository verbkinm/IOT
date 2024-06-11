import QtQuick 2.9
import QtQuick.Controls 2.5

Column {
    required property var action

    width: 400
    spacing: 15

    HostNameComboBox {
        id: srcHost
        height: 50
        width: parent.width
        text: "Имя источника:"
        startText: action.srcHost

        onSignalCurrentTextChanged: (modelIndexText) =>{
           action.srcHost = modelIndexText
        }
    }

    ChannelNumber {
        id: srcChNum
        label: "Канал источника"
        width: parent.width
        value: action.srcChNum

        onSignalNumberChanged: {
            action.srcChNum = value
        }
    }

    HostNameComboBox {
        id: dstHost
        height: 50
        width: parent.width
        text: "Имя приёмника:"
        startText: action.dstHost

        onSignalCurrentTextChanged: (modelIndexText) =>{
           action.dstHost = modelIndexText
        }
    }

    ChannelNumber {
        id: dstChNum
        label: "Канал приёмника"
        width: parent.width
        value: action.dstChNum

        onSignalNumberChanged: {
            action.dstChNum = value
        }
    }
}
