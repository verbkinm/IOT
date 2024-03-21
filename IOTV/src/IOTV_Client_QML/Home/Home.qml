import QtQuick 2.9
import QtQuick.Controls 2.5

Page {
    id: root
    title: "Главная"

    Loader {
        id: loaderHome
        anchors.fill: parent
        source: "Home_Connector.qml"
    }

    Connections {
        target: client
        function onSignalConnected()
        {
            loaderHome.setSource("DeviceListView.qml")
        }
        function onSignalDisconnected()
        {
            glob_deviceStackView.pop(homePage)
            loaderHome.setSource("Home_Connector.qml")
        }
        function onCountDeviceChanged()
        {
            glob_deviceStackView.pop(homePage)
        }
    }

    Component.onCompleted: {
        console.log("Home page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Home page destruct: ", objectName)
    }

    function imageById(id)
    {
        if (id > 0 && id <= 8)
            return "qrc:/img/id/" + id + ".png"

        return "qrc:/img/id/0.png"
    }
}
