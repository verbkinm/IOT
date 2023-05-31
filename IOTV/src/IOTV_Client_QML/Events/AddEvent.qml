import QtQuick 2.9
import QtQuick.Controls 2.5

Page {
    id: root
    title: "Новое событие"

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

            Item {
                height: 50
                width: parent.width

                Text {
                    id: eventTitle
                    text: "Событие:"
                    font.pixelSize: 16

                    anchors {
                        centerIn: parent
                        top: parent.top
                    }
                }
            }

            HostNameComboBox {
                id: hostNameItem
                height: 50
                width: parent.width
            }

            EventType {
                id: eventTypeItem
                height: 50
                width: parent.width
            }

            StateType {
                id: itemStateType
                visible: eventTypeItem.eventType === eventTypeItem.model[2]
                width: parent.width
            }

            DataType {
                id: itemDataType
                visible: eventTypeItem.eventType === eventTypeItem.model[3]
                width: parent.width
            }

            HorizontLine {
                height: 20
                width: parent.width
            }

            // Действия

            Item {
                height: 50
                width: parent.width

                Text {
                    id: actionTitle
                    text: "Действие:"
                    font.pixelSize: 16

                    anchors {
                        centerIn: parent
                        top: parent.top
                    }
                }
            }

            ActionType {
                id: actionTypeItem
                width: parent.width
            }

            ActionDataTX {
                width: parent.width
                visible: actionTypeItem.actiontType === actionTypeItem.model[0]
            }

            ActionDataTX_Ref {
                width: parent.width
                visible: actionTypeItem.actiontType === actionTypeItem.model[1]
            }

            HorizontLine {
                height: 20
                width: parent.width
            }

            Item {
                width: parent.width
                height: 50
                Button {
                    id: save
                    text: "Сохранить"
                    anchors {
                        right: parent.right
                        rightMargin: 20
                        verticalCenter: parent.verticalCenter
                    }

                    onClicked: {

                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Add Events page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Add Events page destruct: ", objectName)
    }
}
