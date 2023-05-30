import QtQuick 2.9
import QtQuick.Controls 2.5

Page {
    id: root
    title: "Новое событие"

    Flickable {
        width: parent.width
        height: parent.height
        contentHeight: 200

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

    }

    Component.onCompleted: {
        console.log("Add Events page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Add Events page destruct: ", objectName)
    }
}
