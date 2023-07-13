import QtQuick 2.9
import QtQuick.Controls 2.2

DialogShared {
    id: dialogExit
    standardButtons: Dialog.Yes | Dialog.No
    title: "Выход"
    text: "Вы действительно хотите выйти?"
    visible: true

    onAccepted: {
        Qt.exit(0)
    }
}
