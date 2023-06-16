import QtQuick 2.9
import QtQuick.Controls 2.5

Column {
    property alias dataString: _data.text
    property alias channelNumber: chNum.value

    property alias hostName: hostNameAction.startHostName

    width: 400//parent.width

    HostNameComboBox {
        id: hostNameAction
        height: 50
        width: parent.width
    }

    DataString {
        id: _data
        width: parent.width
    }

    ChannelNumber {
        id: chNum
        label: "№ канала"
        width: parent.width
    }
}
