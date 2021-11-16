import QtQuick 2.0

Rectangle {
    property alias text: txt.text
    property alias readOnly: txt.readOnly
    property alias inputMask: txt.inputMask

    color: "white";
    border.width: 1;
    border.color: "black";
    radius: 5;
    smooth: true;

    TextInput{
        id: txt
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        maximumLength: 15
        font.pixelSize: 24
        anchors.centerIn: parent

        onFocusChanged: {
            if(focus && !readOnly)
                parent.border.color = "lightblue"
            else
                parent.border.color = "black"
        }
    }
}
