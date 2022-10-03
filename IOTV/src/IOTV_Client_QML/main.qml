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

    property alias appStack: stackView

    Text {
        text: qsTr("Нет подключения к серверу")
        anchors.centerIn: parent
        font.pixelSize: 24
        visible: !client.state && homePage.visible
        z: 1
    }

    header: ToolBar {
        height: 50
        id: overlayHeader

        ToolButton {
            id: pressBack
            text: qsTr("<")

            anchors{
                verticalCenter: parent.verticalCenter
                leftMargin: 20
                left: parent.left
            }
            onClicked: {
                stackView.pop()
            }
            visible: !homePage.visible
        }

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
                name: "Главная"
            }
            ListElement {
                name: "Настройки"
            }
            ListElement {
                name: "Выход"
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
//                                stackView.pop(homePage)
                                stackView.push(clientPage)
                            }
                            else if (index === 2)
                                Qt.quit()

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
        }

        Client {
            id: clientPage
            visible: false
        }
    }

    Connections {
        target: client
        function onSignalDisconnected() {
            if (!client.state && (stackView.currentItem != clientPage && stackView.currentItem != homePage))
                stackView.pop(homePage)
        }
    }

    Popup {
        id: popupWait
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose
        visible: clientPage.connection_attempt

        background: Rectangle{
            opacity: 0.3
        }

        BusyIndicator {
            id: indicator
            antialiasing: true
            anchors.centerIn: parent
            visible: true
            running: true
        }
    }
}

