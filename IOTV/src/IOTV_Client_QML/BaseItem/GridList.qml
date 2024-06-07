import QtQuick 2.15
import "qrc:/BaseItem/" as BaseItem

GridView {
    property alias loader: listLoader
    Loader {
        property string title
        id: listLoader
        objectName: title
        //        property variant modelData: model.get(0)
    }

    id: listView
    cellWidth: global_window.width / 2 - global_window.width * 0.1
    cellHeight: 110

    anchors {
        fill: parent
        leftMargin: parent.width * 0.1
        rightMargin: parent.width * 0.1
        topMargin: 15
    }

    model: ListModel {id: listModel}

    // Анимация появления элементов модели
    populate: Transition {
        NumberAnimation { properties: "x,y"; duration: 1000; easing.type: Easing.OutExpo }
    }
    // Анимация добавления элементов
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 500 }
        NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 500; alwaysRunToEnd: true }
    }
    // Удаление элемента
    remove: Transition {
        PropertyAnimation{ property: "opacity"; to: 0; duration: 500}
        PropertyAnimation{ property: "scale"; to: 0; duration: 500; alwaysRunToEnd: true}
    }

    delegate : BaseItem.ButtonShadow {
        label {
            text: model.text
        }
        icon {
            source: model.icon
        }
        onSignalClicked: {
            if (model.attributes === undefined)
                listLoader.setSource(model.loaderSource)
            else
                listLoader.setSource(model.loaderSource, model.attributes.get(0))

            listLoader.title = model.title
            glob_eventStackView.push(listLoader)
        }

        Component.onCompleted: {
        }
    }
}
