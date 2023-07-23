import QtQuick 2.9
import QtQuick.Controls 2.15

import "qrc:/Devices/BaseItem" as BaseItem

Dialog {
    id: root
    modal: true

    leftMargin: 15
    rightMargin: 15

    width: appStack.width - leftMargin - rightMargin
    height: 100

    standardButtons: Dialog.Close

    visible: false

    onVisibleChanged: {
        if (visible)
            y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - root.height / 2
    }

    Connections {
        target: appStack
        function onHeightChanged() {
            root.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - root.height / 2
        }
        function onWidthChanged() {
            root.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - root.height / 2
        }
    }

    Connections {
        target: global_window
        function onClosing() {
            root.close()
        }
    }
}
