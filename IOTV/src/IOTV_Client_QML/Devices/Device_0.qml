import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    //Ссылка на Device
    required property QtObject device

    id: root
    title: device.aliasName
    objectName: device.aliasName + "_debug"

    Flickable {
        id: fl
        width: root.width
        height: root.height
        contentHeight: column.height + lbl1.height + columnRead.height
                       + lbl2.height + columnWrite.height + 50

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            padding: 10
            spacing: 10

            Label {
                id: name
                text: "Имя устройства: " + device.name
            }
            Label {
                id: aliasName
                width: fl.width
                wrapMode: Text.Wrap
                maximumLineCount: 3
                text: "Псевдоним: " + device.aliasName
            }
            Label {
                id: devId
                text: "ID устройства: " + device.id
            }
            Label {
                id: description
                wrapMode: Text.Wrap
                width: fl.width
                rightPadding: 10
                text: "Описание: " + device.description
            }
            Label {
                id: state
                text: "Состояние: " + (device.state ? "онлайн" : "офлайн")
            }
        }

        Label {
            id: lbl1
            text: "Каналы чтения: "
            anchors {
                top: column.bottom
                left: parent.left
                leftMargin: 10
                topMargin: 10
            }
        }

        Column {
            id: columnRead
            width: root.width
            spacing: 10
            anchors {
                top: lbl1.bottom
                left: parent.left
                right: parent.right
                topMargin: 10
            }

            Repeater {
                id: repeaterRead
                model: device.readChannelLength

                ChannelItem {
                    number: model.index
                    type: device.readDataType(model.index)
                    width: columnRead.width
                    textField.text: device.readData(model.index)
                    textField.readOnly: true

                    buttonIcon: "qrc:/img/copy_white.png"

                    button.onClicked: {
                        textField.selectAll()
                        textField.copy()
                        textField.deselect()

                        glob_notification.set_text("скопировано в буфер")
                    }

                    Connections {
                        target: device
                        function onSignalDataChanged(channel) {
                            if (channel !== model.index)
                                return

                            textField.text = device.readData(model.index)
                        }
                    }
                }
            }
        }

        Label {
            id: lbl2
            text: "Каналы записи: "
            anchors {
                top: columnRead.bottom
                left: parent.left
                leftMargin: 10
                topMargin: 15
                bottomMargin: 5
            }
        }

        Column {
            id: columnWrite
            width: root.width
            spacing: 10
            anchors {
                top: lbl2.bottom
                left: parent.left
                right: parent.right
                topMargin: 10
            }

            Repeater {
                id: repeaterWrite
                model: device.writeChannelLength

                ChannelItem {
                    required property int index
                    number: index
                    type: device.writeDataType(index)
                    width: columnWrite.width

                    buttonIcon: "qrc:/img/send_white.png"
                    button.onClicked: {
                        device.setDataFromString(index, textField.text)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Device 0 construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Device 0 destruct: ", objectName)
    }
}
