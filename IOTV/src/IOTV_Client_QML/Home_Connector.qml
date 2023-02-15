import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.1

Rectangle {
    id: controlConnect
    anchors.fill: parent
    z: 1
    opacity: 1
    visible: true

    Component.onCompleted: {
        console.log(client.state, " ", state)
    }

    states: [
        State {
            name: "hide"; when: (client != null) ? client.state : ""
            PropertyChanges { target: controlConnect; opacity: 0; visible: false }
        },
        State {
            name: "show"; when:  (client != null) ? !client.state : ""
            PropertyChanges { target: controlConnect; opacity: 1; visible: true }
        }
    ]

    transitions: [
        Transition {
            to: "hide"
            ParallelAnimation{
                PropertyAnimation { target: controlConnect; property: "visible"; from: true; to: false; duration: 500 }
                PropertyAnimation { target: controlConnect; property: "opacity"; from: 1; to: 0; duration: 500 }
            }
        },
        Transition {
            to: "show"
            PropertyAnimation { target: controlConnect; property: "opacity"; from: 0; to: 1; duration: 500 }
        }
    ]

    Text {
        id: txtConnection
        text: qsTr("Соединение не установлено")
        anchors.centerIn: parent
        font.pixelSize: 18
        wrapMode: Text.Wrap
    }

    Button {
        id: btnConnect
        width: 180
        font.pixelSize: 18
        text: "подключиться"

        anchors.top: txtConnection.bottom
        anchors.margins: 10
        anchors.horizontalCenter: parent.horizontalCenter

        onClicked: {
            console.log("click")
            clientPage.connectToHost()
        }
    }
}
