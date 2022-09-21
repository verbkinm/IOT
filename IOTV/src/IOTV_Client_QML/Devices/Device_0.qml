import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Page {
    id: root

    //Ссылка на Device
    required property var device

    Flickable {
        id: fl
        width: root.width
        height: root.height
        //        contentHeight: childrenRect.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
//            PropertyAnimation {
//                target: scroll
//                properties: "visible"
//                from: true
//                to: false
//                duration: 5000
//                running: true
//            }
        }

        Column {
            id: column
            padding: 10
            spacing: 10

            Label {id:name}
            Label {id:devId}
            Label {
                id:description
                wrapMode: Text.Wrap
                width: fl.width
                rightPadding: 10
            }
            Label {id:state}
        }

        Label {
            id: lbl1
            text: "Каналы чтения: "
            anchors{
                top: column.bottom
                left: parent.left
                leftMargin: 10
                topMargin: 10
            }
        }

        Column {
            id: columnRead
            width: root.width
            spacing: 5
            anchors {
                top: lbl1.bottom
                left: parent.left
                right: parent.right
                topMargin: 10
            }
        }

        Label {
            id: lbl2
            text: "Каналы записи: "
            anchors{
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
            spacing: 5
            anchors {
                top: lbl2.bottom
                left: parent.left
                right: parent.right
                topMargin: 10
            }
        }
    }

    Component.onCompleted: {
        title  = Qt.binding(function (){ return device.name})
        name.text = Qt.binding(function (){ return "Имя устройства: " + device.name})
        devId.text = Qt.binding(function (){ return "ID устройства: " + device.id})
        description.text = Qt.binding(function (){ return "Описание: " + device.description})
        state.text = Qt.binding(function (){ return "Состояние: " + (device.state ? "онлайн" : "офлайн")})

        for (var i = 0; i < device.readChannelLength; i++)
        {
            var component = Qt.createComponent("ChannelItem.qml");
            if (component.status === Component.Ready)
            {
                var obj = component.createObject(columnRead, {height: 30, number: i, type: device.readDataType(i)})
                obj.width = Qt.binding(function(){return columnRead.width})
                obj.button.text = "✂"
                obj.text = device.readData(i)
            }
        }

        for (i = 0; i < device.writeChannelLength; i++)
        {
            component = Qt.createComponent("ChannelItemWrite.qml");
            if (component.status === Component.Ready)
            {
                obj = component.createObject(columnWrite, {height: 30, number: i, type: device.readDataType(i), _device: device})
                obj.width = Qt.binding(function(){return columnRead.width})
                obj.button.text = "➩"
            }
        }
        timer.start()
    }

    onVisibleChanged: {
        destroy()
    }

    Timer {
        id: timer
        interval: 500
        repeat: true
        running: false
        onTriggered: {
            var children = columnRead.children
            for (var i = 0; i < children.length; i++)
                children[i].text = device.readData(i)

            fl.contentHeight = column.height + lbl1.height + columnRead.height + lbl2.height + columnWrite.height + 50
        }
    }
}
