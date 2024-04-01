import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.15

import "Home" as HomePageModule
import "Client" as ClientPageModule
import "Events" as EventsPageModule

ApplicationWindow {
    id: global_window
    width: 400
    height: 640
    visible: true
    title: qsTr("IOTV Client")

    minimumWidth: 400
    minimumHeight: 400

    //! [orientation]
    readonly property bool inPortrait: global_window.width < global_window.height
    //! [orientation]

    property alias glob_swipeView: swipeView

    property alias glob_deviceStackView: stackView_1
    property alias glob_eventStackView: stackView_2
    property alias glob_clientStackView: stackView_3

    property alias glob_dialogShared: dialogShared
    property alias glob_notification: notification


    header: ToolBar {
        height: 50
        id: overlayHeader

        ToolButton {
            id: pressBack
            icon {
                color: "transparent"
                source: "qrc:/img/back.png"
            }

            anchors {
                verticalCenter: parent.verticalCenter
                leftMargin: 20
                left: parent.left
            }
            onClicked: {
                if (drawer.visible)
                    drawer.visible = false
                else
                    global_window.close()
            }
            visible: {
                switch(glob_swipeView.currentIndex)
                {
                case 0: return (glob_deviceStackView.currentItem != homePage)
                case 1: return (glob_eventStackView.currentItem != eventsPage)
                case 2: return false
                }
                return false
            }
        }

        Label {
            width: parent.width
            height: parent.height
            text: {
                switch(glob_swipeView.currentIndex)
                {
                case 0: return glob_deviceStackView.currentItem.title
                case 1: return glob_eventStackView.currentItem.title
                case 2: return glob_clientStackView.currentItem.title
                }
                return "Title"
            }
            elide: Text.ElideRight
            font.pixelSize: 18
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            leftPadding: pressBack.width + pressBack.anchors.leftMargin
            rightPadding: pressBack.width + pressBack.anchors.leftMargin
        }

        ToolButton {
            id: menu
            icon {
                color: "transparent"
                source: "qrc:/img/menu.png"
            }

            anchors {
                verticalCenter: parent.verticalCenter
                rightMargin: 20
                right: parent.right
            }
            onClicked: {
                drawer.visible = !drawer.visible
            }
        }
    }

    MainMenu {
        id: drawer
        y: overlayHeader.height
        onClosed: glob_swipeView.focus = true // для glob_swipeView Keys.onEscapePressed:
    }

    SwipeView{
        id: swipeView

        currentIndex: 0
        anchors.fill: parent

        Keys.onEscapePressed: (event) =>{
                                  global_window.close()
                              }

        onCurrentIndexChanged: {
            glob_swipeView.focus = true // для glob_swipeView Keys.onEscapePressed:
        }

        // Список устройств
        Item {
            StackView {
                id: stackView_1
                anchors.fill: parent
                initialItem: homePage

                onCurrentItemChanged: {
                    if (stackView_1.currentItem == homePage)
                    {
                        homePage.destroyDev()
                    }

                    console.log("stackView_1 current item: ", stackView_1.currentItem.objectName)
                    glob_swipeView.focus = true // для glob_swipeView Keys.onEscapePressed:
                }

                HomePageModule.Home {
                    id: homePage
                    objectName: "Home"
                }
            }
        }

        // Список событий
        Item {
            StackView
            {
                id: stackView_2
                anchors.fill: parent
                initialItem: eventsPage

                onCurrentItemChanged: {
                    if (stackView_2.currentItem == eventsPage)
                    {
                        eventsPage.destroyEv()
                    }

                    console.log("stackView_2 current item: ", stackView_2.currentItem.objectName)
                    glob_swipeView.focus = true // для glob_swipeView Keys.onEscapePressed:
                }

                EventsPageModule.Events {
                    //                    anchors.fill: parent
                    id: eventsPage
                    objectName: "Events"

                }
            }
        }

        // Настройки подключения
        Item {
            StackView
            {
                id: stackView_3
                anchors.fill: parent
                initialItem: clientPage

                onCurrentItemChanged: {
                    console.log("stackView_3 current item: ", stackView_3.currentItem.objectName)
                    glob_swipeView.focus = true // для glob_swipeView Keys.onEscapePressed:
                }

                ClientPageModule.Client {
                    anchors.fill: parent
                    id: clientPage
                    objectName: "Client"
                }
            }
        }
    }

    PageIndicator {
        id: indicator

        count: glob_swipeView.count
        currentIndex: glob_swipeView.currentIndex

        anchors.bottom: glob_swipeView.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    footer: ToolBar {
        height: overlayHeader.height
        id: overlayFooter

        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon {
                    color: "transparent"
                    source: "qrc:/img/home.png"
                }
                onClicked: {
                    glob_swipeView.setCurrentIndex(0)
                }
                Layout.alignment: Qt.AlignCenter
            }

            ToolButton {
                icon {
                    color: "transparent"
                    source: "qrc:/img/calendar_white.png"
                }
                onClicked: {
                    glob_swipeView.setCurrentIndex(1)
                }
                //                }
                Layout.alignment: Qt.AlignCenter
            }

            ToolButton {
                icon {
                    color: "transparent"
                    source: "qrc:/img/settings_white.png"
                }
                onClicked: {
                    glob_swipeView.setCurrentIndex(2)
                }
                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    // Для глобальных уведомлений
    Notification {
        id: notification
    }

    // Для глобальных диалогов
    DialogShared {
        id: dialogShared
        visible: false
    }

    onClosing: (close)=> {
                   close.accepted = false

                   glob_dialogShared.defaultAcceptedExit()

                   if (drawer.visible)
                   {
                       drawer.visible = 0
                   }
                   else if (glob_dialogShared.visible)
                   {
                       glob_dialogShared.close()
                   }
                   else if (glob_swipeView.currentIndex === 0)
                   {
                       if (glob_deviceStackView.currentItem == homePage) glob_dialogShared.open()
                       else glob_deviceStackView.pop()
                   }
                   else if (glob_swipeView.currentIndex === 1)
                   {
                       if (glob_eventStackView.currentItem == eventsPage) glob_swipeView.setCurrentIndex(0)
                       else glob_eventStackView.pop()
                   }
                   else if (glob_swipeView.currentIndex === 2)
                   {
                       glob_swipeView.setCurrentIndex(0)
                   }
               }

    Component.onCompleted: {

    }
}
