import QtQuick 2.9

Item {
    Image {
        id: img
        source: "file:/home/user/myProg/IOT/build-IOTV-Desktop-Debug/src/IOTV_Client_QML/Image.jpg"
        //            width: 200
        //            height: 200
        sourceSize.width: 200
        sourceSize.height: 200

        MouseArea {
            anchors.fill: parent
            onClicked: {
                switchStream++
                if (switchStream % 2 == 0)
                {
                    console.log("close stream")
                    device.signalCloseReadStream(0)
                }
                else
                {
                    console.log("open stream")
                    device.signalOpenReadStream(0)
                }
            }
        }
    }
}
