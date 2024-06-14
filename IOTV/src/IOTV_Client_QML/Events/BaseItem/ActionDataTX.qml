import QtQuick 2.9
import QtQuick.Controls 2.5

Column {
    required property var action

    width: 400
    spacing: 15

    HostNameComboBox {
        id: hostNameAction
        height: 50
        width: parent.width

        startText: action.hostName//client.aliasName(action.hostName)

        onSignalCurrentTextChanged: (modelIndexText) =>{
           action.hostName = modelIndexText
        }
    }

    DataString {
        id: _data
        width: parent.width
        text: action.dataStr

        onSignalTextEdited: {
            action.dataStr = text
        }
    }

    ChannelNumber {
        id: chNum
        label: "№ канала"
        width: parent.width
        value: action.chNum

        onSignalNumberChanged: {
            action.chNum = value
        }
    }
}
