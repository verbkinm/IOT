import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
import QtQuick.Extras 1.4


Page {
    id: root
    property bool status: false

    property int countDevice: 0
    property int countDeviceOnline: 0

    title: "Home"

    signal mySignal()

//    CircularGauge {
//        maximumValue: 50
//        minimumValue: -50
//        value: accelerating ? maximumValue : minimumValue
//        anchors.centerIn: parent

//        property bool accelerating: false

//        Keys.onSpacePressed: accelerating = true
//        Keys.onReleased: {
//            if (event.key === Qt.Key_Space) {
//                accelerating = false;
//                event.accepted = true;
//            }
//        }

//        Component.onCompleted: forceActiveFocus()

//        Behavior on value {
//            NumberAnimation {
//                duration: 1000
//            }
//        }
//    }

    Flickable
    {
        id: flickable_home
        anchors.fill: parent
        contentHeight: childrenRect.height

//        ScrollBar.vertical: ScrollBar { }




//        MessageDialog {
//            id: messageDialog
//            title: "May I have your attention please"
//            text: "It's so cool that you are using Qt Quick."
//            onAccepted: {
//                console.log("And of course you could only agree.")
//            }
//            Component.onCompleted: visible = true
//        }


        Label {
            id: label1
            text: "Состояние: " + (status ? "подключён" : "отключён")
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.topMargin: 10
            anchors.top: parent.top
        }

        Label {
            text: "Количество устройств: (" + countDeviceOnline + "/" + countDevice + ")"
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.topMargin: 10
            anchors.top: label1.bottom
        }
//        ToggleButton {
//            anchors.centerIn: parent
//            Component.onCompleted: clicked()
//            onClicked: {
//                if (checked)
//                {
//                    text = "on"
//                    root.mySignal()
//                }
//                else
//                    text = "off"
//            }
//        }
    }
}
