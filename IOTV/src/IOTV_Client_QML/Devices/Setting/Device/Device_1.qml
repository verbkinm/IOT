import QtQuick 2.9
import QtQuick.Controls 2.2
import Qt.labs.settings 1.1

Rectangle {
    //Ссылка на Device
    required property var device

    id: root
    width: parent.width

    color: Qt.rgba(0, 0, 0, 0)

    ListView {
        id: listView
        interactive: false

        anchors {
            fill: parent
            topMargin: 15
        }

        model: listModel
        delegate: contactDelegate

        spacing: 15
    }

    ListModel {
        id: listModel
    }

    Component {
        id: contactDelegate

        Rectangle {
            id: componentRect
            width: listView.width
            height: 52
            color: Qt.rgba(0, 0, 0, 0)

//            Label {
//                id: lbl
//                wrapMode: Text.Wrap
//                text: "Канал : " + model.index
//                font.pixelSize: 14
//                elide: Text.ElideRight

//                anchors{
//                    left: parent.left
//                    verticalCenter: parent.verticalCenter
//                    leftMargin: 15
//                    rightMargin: 15
//                }
//            }

            TextField {
                id: txt
                antialiasing: true
                font.pixelSize: 14
                placeholderText: "Канал : " + model.index
                horizontalAlignment: Text.AlignHCenter

                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    leftMargin: 15
                    rightMargin: 15
                }
            }

            Settings {
                id: settings
                category: device.name + "_channel_" + model.index
                property alias name: txt.text
            }
        }
    }

    Component.onCompleted: {
        for( var i = 0; i < device.writeChannelLength; i++)
        {
            var object = {}
            listModel.append(object)
        }
        root.height = listModel.count * 40 + listView.anchors.topMargin + listView.spacing * listModel.count
    }
}
