import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    width: 640
    minimumWidth: 640
    height: 480
    minimumHeight: 480
    visible: true
    title: qsTr("IOTV-Клиент")

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Server_Page {
            onClick: {
//                console.log(back.objectName)
            }
            onDisconected: {
                host_page.clear()
            }

            back.onSignalDevicesCreated: {
                host_page.objectsArray = back.getDevicesToQML()
                host_page.createDivecGUI()
            }
//            back.onTotalDeviceChanged: {
//                host_page.clear()
//                host_page.createDivecGUI()
//            }
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
