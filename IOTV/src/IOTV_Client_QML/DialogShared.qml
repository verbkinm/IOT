import QtQuick 2.2
import QtQuick.Controls 2.2

Dialog {
    property alias text: lbl.text
    property variant func_accepted: function(){}

    id: dialog
    modal: true

    width: parent.width * 0.8

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    Label {
        id: lbl
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 18
        wrapMode: Text.Wrap
    }

    onAccepted: func_accepted()

    Component.onCompleted: {
        console.log("construct DialogShared")
    }

    Component.onDestruction: {
        console.log("destruct DialogShared")
    }

    function defaultAcceptedExit()
    {
        func_accepted = function (){
            glob_deviceStackView.pop(homePage)
            Qt.exit(0)
        }
        standardButtons = Dialog.Yes | Dialog.No
        title = "Выход"
        text = "Вы действительно хотите выйти?"
    }

    function defaultAcceptedMessage()
    {
        func_accepted = function (){close()}
        standardButtons = Dialog.Ok
        title = "Title"
        text = "Text"
    }

    function defaultAcceptedDo(doFunc)
    {
        func_accepted = doFunc
    }
}
