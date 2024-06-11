import QtQuick 2.9
import QtQuick.Controls 2.5
import "qrc:/Events/BaseItem" as BaseItem

Page {
    id: root

    property string oldGroupName: "" // при изменении имени существующей группы
    property alias btnDeleteVisible: deleteBtn.visible

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
                text: oldGroupName === "" ? "Новая группа действий" : oldGroupName
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
                    id: deleteBtn
                    width: 64
                    height: 64
                    highlighted: true

                    anchors {
                        right: save.left
                        rightMargin: 20
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/delete_white.png"
                        height: 24
                        width: 24
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        if (!client.isEmptyActionGroup(oldGroupName))
                        {
                            glob_dialogShared.defaultAcceptedMessage()
                            glob_dialogShared.title = "Внимание"
                            glob_dialogShared.text = "Нельзя удалить группу, которая содержить объекты!"
                            glob_dialogShared.open()

                            return
                        }

                        client.removeActionGroup(oldGroupName)
                        glob_eventStackView.pop()
                        glob_eventStackView.pop()
                    }
                }

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

                        // Если добавляем новую группу
                        if (oldGroupName === "" && client.isExistsActionGroup(groupName.text))
                        {
                            groupIsExist()
                            return
                        }

                        // Если изменяем имя существующей группы
                        if (oldGroupName !== "")
                        {
                            if (client.isExistsActionGroup(groupName.text))
                            {
                                groupIsExist()
                            }
                            else
                            {
                                client.renameActionGroup(oldGroupName, groupName.text)
                                glob_eventStackView.pop()
                                glob_eventStackView.pop()
                            }
                            return
                        }

                        client.saveActionGroup(groupName.text)
                        glob_eventStackView.pop()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Add Action group page construct: ")
    }

    Component.onDestruction: {
        console.log("Add Action group page destruct: ", objectName)
    }

    function groupIsExist()
    {
        glob_dialogShared.defaultAcceptedMessage()
        glob_dialogShared.title = "Внимание"
        glob_dialogShared.text = "Такое имя группы действий уже существует!"
        glob_dialogShared.open()
    }
}
