import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    width: 600
    height: 400

    property bool status: false

    header: Label {
        text: qsTr("Хосты")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
}
