import QtQuick 2.9
import QtQuick.Controls 2.5
import "qrc:/BaseItem" as BaseItem

Rectangle {
    height: 64

    color: Qt.rgba(255, 255, 255, 1)
    border.width: 1
    border.color: Qt.rgba(0, 0, 0, 0.1)

    BaseItem.Led_Indicator {
        ledEnable: device.state
        anchors {
            left: parent.left
            leftMargin: parent.height / 2
            verticalCenter: parent.verticalCenter
        }
    }

    RoundButton {
        id: settings

        width: 52
        height: 52
        highlighted: true

        anchors {
            right: debugMode.left
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
            loaderDebug.objectName = device.aliasName + "_setting"
            loaderDebug.setSource("/Devices/Setting/Setting.qml", {
                                      "device": device
                                  })
            glob_deviceStackView.push(loaderDebug)
        }
    }

    RoundButton {
        id: debugMode

        width: settings.width
        height: settings.height
        highlighted: true

        anchors {
            right: info.left
            verticalCenter: parent.verticalCenter
            rightMargin: 10
        }
        Image {
            anchors.centerIn: parent
            source: "qrc:/img/debug_white.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit // ensure it fits
        }

        onClicked: {
            loaderDebug.objectName = device.aliasName + "_debug"
            loaderDebug.setSource("/Devices/Device_0.qml", {
                                      "device": device
                                  })
            glob_deviceStackView.push(loaderDebug)
        }
    }

    RoundButton {
        id: info

        width: settings.width
        height: settings.height
        highlighted: true

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: 10
        }
        Image {
            anchors.centerIn: parent
            source: "qrc:/img/question_mark_white.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit // ensure it fits
        }

        onClicked: {
            glob_dialogShared.defaultAcceptedMessage()
            glob_dialogShared.title = "Описание"
            glob_dialogShared.text = device.description
            glob_dialogShared.open()
        }
    }

    Loader {
        property string title: device.aliasName

        id: loaderDebug
        objectName: "debug"

        onVisibleChanged: {
            if (this.visible === false)
            {
                loaderDebug.setSource("")
            }
        }
    }
}
