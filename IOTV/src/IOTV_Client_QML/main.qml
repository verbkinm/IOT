import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3
import Qt.labs.platform 1.1

ApplicationWindow {

    id: window
    width: 360
    height: 520
    visible: true
    title: qsTr("IOTV Client")
    minimumWidth: 360
    minimumHeight: 520

    //! [orientation]
    readonly property bool inPortrait: window.width < window.height
    //! [orientation]

    header: ToolBar {
        height: 50
        id: overlayHeader
        RowLayout {
            anchors.fill: parent
            Label {
                text: stackView.currentItem.title
                font.pixelSize: 18
                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    footer: ToolBar {
        height: 50
        id: overlayFooter
        RowLayout {
            anchors.fill: parent
            ToolButton {
                id: pressBack
                text: qsTr("<")
                onClicked: {
                    stackView.pop()
                }
                Layout.alignment: Qt.AlignCenter
            }
            ToolButton {
                text: qsTr("🏠")
                onClicked: {
                    drawer.visible = false
                    stackView.pop(homePage)
                }
                Layout.alignment: Qt.AlignCenter
            }
            ToolButton {
                text: qsTr("☰")
                onClicked: {
                    drawer.visible = !drawer.visible
                }
                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    Drawer {
        id: drawer

        y: overlayHeader.height
        width: window.width / 2
        height: window.height - overlayHeader.height

        modal: inPortrait
        interactive: inPortrait
        position: inPortrait ? 0 : 1
        visible: !inPortrait


        ListModel {
            id: listModel

            ListElement {
                name: "Home"
            }
            ListElement {
                name: "Server"
            }
        }

        Component {
            id: delegate
            Item {
                width: parent.width; height: 30
                Rectangle {
                    anchors.fill: parent
                    color: "transparent"

                    MouseArea{
                        anchors.fill: parent
                        anchors.centerIn: parent
                        Text {
                            anchors.centerIn: parent
                            text: name;
                            font.pointSize: 12
                        }
                        onClicked: {
                            if (index === 0)
                                stackView.pop(homePage)
                            else if (index === 1 && stackView.currentItem != clientPage)
                            {
                                stackView.pop(homePage)
                                stackView.push(clientPage)
                            }

                            drawer.visible = 0
                        }
                    }
                }
            }
        }

        ListView {
            id: listView
            anchors.fill: parent
            model: listModel
            delegate: delegate
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: homePage

        Home {
            id: homePage

            onSignalOpenDevice: function(name) {
                var component = Qt.createComponent("/Devices/Device_0.qml");
                if (component.status === Component.Ready)
                {
                    var dev = client.deviceByName(name)
                    var obj = component.createObject(window, {device: dev})
                    stackView.push(obj);
                    dev.signalUpdate.connect(function() {pressBack.clicked()})
                }
            }
        }

        Client {
            id: clientPage
            visible: false
        }        
    }

    Popup {
        id: popupWait
        anchors.centerIn: parent
        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose
        opacity: 0.0
        visible: clientPage.connection_attempt

        BusyIndicator {
            id: indicator
            antialiasing: true
            anchors.centerIn: parent
        }
    }
}

