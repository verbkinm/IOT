#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->hexText, &QPlainTextEdit::textChanged, this, &MainWindow::slotConvert);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::printHeaderData(Header *header)
{
    Q_ASSERT(header != nullptr);
    QString result;

    result += "Версия протокола:\t" + QString::number(header->version) + "\n";
    result += "Тип пакета\t\t" + QString::number(header->type) + "\n";
    result += "Назначение пакета \t" + QString::number(header->assignment) + "\n";
    result += "Флаги\t\t" + QString::number(header->flags) + "\n";
    result += "Размер данных\t" + QString::number(header->dataSize) + "\n";

    ui->convertText->appendPlainText(result);
}


void MainWindow::on_action_triggered()
{
    QApplication::quit();
}

void MainWindow::slotConvert()
{
    QString result;
    QByteArray rawData;

    for (auto &str : ui->hexText->toPlainText().split(":"))
    {
        QByteArray data(str.toLatin1());
        bool ok;
        int hex = data.toInt(&ok, 16);     // hex == 255, ok == true
        if (!ok)
        {
            ui->convertText->setPlainText("Ошибка конверитрования!");
            return;
        }

        rawData.append(hex);

        result += QString::number(hex) + " ";
    }
    ui->convertText->setPlainText(result);

    bool error;
    uint64_t expectedDataSize = 0, cutDataSize = 0;

    auto header = createPkgs((uint8_t *)rawData.toStdString().c_str(), rawData.size(), &error,
                             &expectedDataSize, &cutDataSize);

    if (error)
    {
        ui->convertText->setPlainText("Ошибка данных");
        return;
    }

    ui->convertText->setPlainText("HEADER\n");

    if (header == nullptr)
    {
        if (error)
        {
            ui->convertText->setPlainText("Ошибка конверитрования!");
            return;
        }
        else if(expectedDataSize > 0)
        {
            ui->convertText->appendPlainText("Не полные данные");
            return;
        }
    }

    printHeaderData(header);
}
