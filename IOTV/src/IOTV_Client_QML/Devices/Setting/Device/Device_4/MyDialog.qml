import QtQuick 2.9
import QtQuick.Controls 2.15

import "qrc:/Devices/BaseItem" as BaseItem

Dialog {
    id: root
    modal: true

    leftMargin: 15
    rightMargin: 15

    width: glob_deviceStackView.width - leftMargin - rightMargin
    height: 100

    standardButtons: Dialog.Close

    visible: false

    onVisibleChanged: {
        if (visible)
            y = mapFromItem(glob_deviceStackView, 0, 0).y + glob_deviceStackView.height / 2 - root.height / 2
    }

    Connections {
        target: glob_deviceStackView
        function onHeightChanged() {
            root.y = mapFromItem(glob_deviceStackView, 0, 0).y + glob_deviceStackView.height / 2 - root.height / 2
        }
        function onWidthChanged() {
            root.y = mapFromItem(glob_deviceStackView, 0, 0).y + glob_deviceStackView.height / 2 - root.height / 2
        }
    }

    Connections {
        target: global_window
        function onClosing() {
            root.close()
        }
    }
}
