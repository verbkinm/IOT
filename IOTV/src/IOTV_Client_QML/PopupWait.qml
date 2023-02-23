import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

//import "Home" as HomePageModule
//import "Client" as ClientPageModule

Popup {
    objectName: "Popup"
    anchors.centerIn: parent
    width: parent.width
    height: parent.height
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose
    visible: true

    background: Rectangle{
        opacity: 0.2
    }

    BusyIndicator {
        id: indicator
        antialiasing: true
        anchors.centerIn: parent
        visible: true
        running: true
    }
}
