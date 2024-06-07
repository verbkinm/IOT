import QtQuick 2.9
import QtQuick.Controls 2.5
import "qrc:/Devices/BaseItem" as BaseItem

Rectangle {
    height: 64
    width: parent.width

    color: Qt.rgba(255, 255, 255, 1)
    border.width: 1
    border.color: Qt.rgba(0, 0, 0, 0.1)

    signal signalClicked()

    RoundButton {
        id: settings

        width: 52
        height: 52
        highlighted: true

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: 10
        }
        Image {
            anchors.centerIn: parent
            source: "qrc:/img/settings_white.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit // ensure it fits
        }

        onClicked: {
            signalClicked()
//            loaderDebug.objectName = device.aliasName + "_setting"
//            loaderDebug.setSource("/Devices/Setting/Setting.qml", {
//                                      "device": device
//                                  })
//            glob_deviceStackView.push(loaderDebug)
        }
    }
}
