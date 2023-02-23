import QtQuick 2.9
import QtQuick.Controls 2.5

Rectangle {
    id: controlConnect
    anchors.fill: parent
    opacity: 1
    visible: true

    readonly property string stateHide: "hide"
    readonly property string stateShow: "show"

    states: [
        State {
            name: stateHide
            when: (client != null) ? client.state : ""
            PropertyChanges { target: controlConnect; opacity: 0; visible: false }
        },
        State {
            name: stateShow
            when:  (client != null) ? !client.state : ""
            PropertyChanges { target: controlConnect; opacity: 1; visible: true }
        }
    ]

    transitions: [
        Transition {
            to: stateHide
            ParallelAnimation{
                PropertyAnimation { target: controlConnect; property: "visible"; from: true; to: false; duration: 500 }
                PropertyAnimation { target: controlConnect; property: "opacity"; from: 1; to: 0; duration: 500 }
            }
        },
        Transition {
            to: stateShow
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

    // Дублирует clientPage.btn
    Button {
        width: clientPage.btn.width
        font.pixelSize: clientPage.btn.font.pixelSize
        text: clientPage.btn.text

        anchors.top: txtConnection.bottom
        anchors.margins: 10
        anchors.horizontalCenter: parent.horizontalCenter

        onClicked: {
            clientPage.btn.clicked()
        }
    }
}
