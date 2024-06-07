import QtQuick 2.9
import QtQuick.Controls 2.5
import "qrc:/Events/BaseItem" as BaseItem

Page {
    id: root

    Flickable {
        width: parent.width
        height: parent.height

        contentHeight: column.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            anchors.top: parent.top
            anchors.topMargin: 15
            spacing: 15

            TextField {
                id: groupName
                focus: true
                height: 52
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                antialiasing: true
                font.pixelSize: 14
                text: "Новая группа событий"
                placeholderText: "Введите название группы..."
                placeholderTextColor: "#ccc"

                anchors {
                    right: parent.right
                    left: parent.left
                    margins: 20
                }
            }

            Item {
                width: parent.width
                height: 70

                RoundButton {
                    id: save
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        right: parent.right
                        rightMargin: 20
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/save_white.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        if (groupName.text === "")
                        {
                            glob_dialogShared.defaultAcceptedMessage()
                            glob_dialogShared.title = "Внимание"
                            glob_dialogShared.text = "Имя группы не может быть пустым!"
                            glob_dialogShared.open()

                            return
                        }

                        if (client.isExistsEventGroup(groupName.text))
                        {
                            glob_dialogShared.defaultAcceptedMessage()
                            glob_dialogShared.title = "Внимание"
                            glob_dialogShared.text = "Такое имя группы событий уже существует!"
                            glob_dialogShared.open()

                            return
                        }

                        client.saveEventGroup(groupName.text)
                        glob_eventStackView.pop()

//                        client.saveEvent(_event)
//                        client.queryEventAction()

//                        glob_eventStackView.pop(eventsPage)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Add Event group page construct: ")
    }

    Component.onDestruction: {
        console.log("Add Event group page destruct: ", objectName)
    }
}
