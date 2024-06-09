import QtQuick 2.9
import QtQuick.Controls 2.5

Rectangle {
    property alias text: lbl.text
    property alias icon: imgId.source

    height: 64
    width: parent.width

    color: Qt.rgba(255, 255, 255, 1)
    border.width: 1
    border.color: Qt.rgba(0, 0, 0, 0.1)

    signal signalClicked()

    Image {
        id: imgId
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: 10
        }
        source: "qrc:/img/folder.png"
        height: 52
        width: 52
        fillMode: Image.PreserveAspectFit // ensure it fits
    }

    Label {
        id: lbl
        text: "text"

        anchors {
            left: imgId.right
            verticalCenter: parent.verticalCenter
            leftMargin: 10
        }
    }

    RoundButton {
        id: settings

        width: 52
        height: 52
        highlighted: true

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: 10
        }
        Image {
            anchors.centerIn: parent
            source: "qrc:/img/settings_white.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit // ensure it fits
        }

        onClicked: {
            signalClicked()
        }
    }
}
