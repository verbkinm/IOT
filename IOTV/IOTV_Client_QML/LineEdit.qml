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
        text: "text"
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        focus: true
        antialiasing: true
        anchors.horizontalCenter: parent.horizontalCenter
        maximumLength: 15
        font.pixelSize: 18

        onFocusChanged: {
            if(focus && !readOnly)
                parent.border.color = "lightblue"
            else
                parent.border.color = "black"
        }
    }
}
