import QtQuick 2.0

Rectangle {
    id: root

    property alias text: txt.text
    property alias readOnly: txt.readOnly
    property alias inputMask: txt.inputMask
    property bool active: true

    color: "white";
//    gradient: Gradient {
//        GradientStop {
//            position: 0.30;
//            color: "#ffffff";
//        }
//        GradientStop {
//            position: 1.00;
//            color: "#000000";
//        }
//    }
    border.width: 1;
    border.color: "black";
    radius: 3;
    smooth: true;

    TextInput{
        id: txt
        text: "text"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        focus: true
        antialiasing: true
        anchors.horizontalCenter: parent.horizontalCenter
        maximumLength: 15
        font.pixelSize: 18
        font.family: "Open Sans"
        color: "#434a54"

        onFocusChanged: {
            if(focus && !readOnly)
                parent.border.color = "lightblue"
            else
                parent.border.color = "black"
        }
    }
    onActiveChanged: {
        if (active)
        {
            txt.readOnly = false
            root.color = "white"
        }
        else
        {
            txt.readOnly = true
            root.color = "gainsboro"
//            root.border.color =
        }
    }
}
