import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    minimumWidth: 480
    minimumHeight: 40
    visible: true
    title: qsTr("IOTV-Клиент")

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Server_Page {
            id: server
//            anchors.fill: parent

            onDisconected: {
                host_page.clear()
            }

            back.onSignalDevicesCreated: {
                host_page.objectsArray = back.getDevicesToQML()
                host_page.createDivecGUI()
            }
        }

        Hosts_Page {
            id: host_page
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Сервер")
        }
        TabButton {
            text: qsTr("Устройства")
        }
    }
}
