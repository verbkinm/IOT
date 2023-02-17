import QtQuick 2.9
import QtQuick.Controls 2.5

Page {
    id: root
    title: "Главная"

    footer: Item {
        height: 20
        width: parent.width

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 5
            font.pixelSize: 12
            text: "Состояние: " +
                  (client.state ? "подключено" : "отключено") +
                  ", кол-во устройств: (" + client.onlineDevice + "/" + client.totalDevice + ")"
            wrapMode: Text.Wrap
        }
    }

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
            loaderHome.setSource("Home_Connector.qml")

            //!!!
            var flag = appStack.currentItem == clientPage
            appStack.pop(homePage)

            if (flag)
                appStack.push(clientPage)
        }
    }

    function imageById(id)
    {
        if (id === 1)
            return "qrc:/img/id/1.png"
        else if (id === 2)
            return "qrc:/img/id/2.png"
        else if (id === 3)
            return "qrc:/img/id/3.png"
        else if (id === 4)
            return "qrc:/img/id/4.png"
        else
            return "qrc:/img/id/0.png"
    }
}
