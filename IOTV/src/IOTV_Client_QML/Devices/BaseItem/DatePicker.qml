import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialogCalendar
    modal: true
    standardButtons: Dialog.Save | Dialog.Cancel

    width: 400
    height: 300

    signal dateChanged(date date)

    required property date selectedDate

    anchors.centerIn: parent

    onAccepted: {
        dateChanged(selectedDate)
    }


    Dialog {
        id: dialogMonthYear

        modal: true
        standardButtons: Dialog.Save | Dialog.Cancel

        anchors.centerIn: parent

        width: parent.parent.width
        height: item1.height + 70

        signal monthYearChanged(int month, int year)

        Component.onCompleted: {
            var model = []
            for (var i = 1; i <= 12; i++)
                model.push(i)

            monthThumbler.model = model

            model = []
            for (i = 2000; i <= 2030; i++)
                model.push(i)

            yearThumbler.model = model

            monthThumbler.currentIndex = selectedDate.getMonth()
            yearThumbler.currentIndex = selectedDate.getFullYear() - 2000
        }

        onAccepted: {
            grid.month = monthThumbler.currentIndex
            grid.year = yearThumbler.currentIndex + 2000
        }

        Item {
            id: item1
            height: 180
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            Row {
                anchors.centerIn: parent
                id: tumblerRow
                Tumbler {
                    id: monthThumbler
                    width: 80
                    height: 145
                    wheelEnabled: false
                    spacing: 0
                    visibleItemCount: 3
                    font.pixelSize: 18

                    Label {
                        font.pixelSize: 18
                        text: "/"
                        anchors {
                            verticalCenter: monthThumbler.verticalCenter
                            left: monthThumbler.right
                        }
                    }
                }

                Tumbler {
                    id: yearThumbler
                    width: 80
                    height: 145
                    visibleItemCount: 3
                    font.pixelSize: 18
                }
            }
            Rectangle {
                id: tumblerRect
                width: parent.width * 0.9
                height: 60
                color: "#11000000"
                radius: 25
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

    }

    GridLayout {
        width: parent.width
        columns: 2

        Label {
            id: calenarMonth
            text: grid.title + " ▼"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            Layout.columnSpan: 2
            Layout.fillWidth: true

            font.pixelSize: 18

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    dialogMonthYear.open()
                }
            }
        }

        DayOfWeekRow {
            locale: grid.locale

            Layout.column: 1
            Layout.row: 1
            Layout.fillWidth: true

            font.pixelSize: 18
        }

        WeekNumberColumn {
            id: monthGrid
            month: grid.month
            year: grid.year
            locale: grid.locale

            Layout.fillHeight: true
        }

        MonthGrid {
            id: grid
            month: new Date().getMonth()
            year: new Date().getFullYear()
            locale: Qt.locale("ru_RU")

            Layout.fillWidth: true
            Layout.fillHeight: true

            delegate: Rectangle {
                required property var model

                //                color: model.date.toDateString() === selectedDate.toDateString() && model.month === grid.month ? Qt.color("blue") : Qt.rgba(0, 0, 0, 0)
                border.color: "black"
                border.width:  model.date.toDateString() === selectedDate.toDateString() && model.month === grid.month ? 1 : 0
                radius: 15

                Text {
                    id: text
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: grid.locale.toString(model.date, "d")
                    opacity: model.month === grid.month ? 1 : 0.5
                    font.pixelSize: 18
                }
            }
            onClicked: (date) => {
                           var lastSelectedDAte = selectedDate
                           selectedDate = date

                           if (selectedDate.getMonth() !== grid.month)
                           selectedDate = lastSelectedDAte
                       }
        }
    }
}
