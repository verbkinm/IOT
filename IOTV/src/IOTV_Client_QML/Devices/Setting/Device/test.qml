import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    ComboBox {
        id: comboBox
        x: 272
        y: 104

        model: 6
    }

    Text {
        id: text1
        x: 137
        y: 120
        text: qsTr("Text")
        font.pixelSize: 12
    }

    Rectangle {
        id: rectangle
        x: 106
        y: 212
        width: 98
        height: 98
        color: "red"
        radius: width / 2
    }

    Rectangle {
        id: rectangle1
        x: 396
        y: 212
        width: 98
        height: 98
        color: "green"
        radius: width / 2
    }

    Rectangle {
        id: rectangle2
        x: 243
        y: 212
        width: 98
        height: 98
        color: "blue"
        radius: width / 2
    }

    CheckBox {
        id: checkBox
        x: 134
        y: 236
        checked: true
    }

    CheckBox {
        id: checkBox1
        x: 272
        y: 236
        checked: true
    }

    CheckBox {
        id: checkBox2
        x: 424
        y: 236
        checked: true
    }
}
