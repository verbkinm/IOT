#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(ui->hexText);
    splitter->addWidget(ui->convertText);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(new QLabel("HEX данные:", this), 0, 0);
    layout->addWidget(new QLabel("Конвертированные данные:", this), 0, 1);
    layout->addWidget(splitter, 1, 0, 1, 2);
    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 1);

    ui->centralwidget->setLayout(layout);

    connect(ui->hexText, &QPlainTextEdit::textChanged, this, &MainWindow::slotConvert);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::printHeaderData(const Header *header)
{
    Q_ASSERT(header != nullptr);
    QString result;

    result += "Версия протокола:\t" + QString::number(header->version) + "\n";
    result += "Тип пакета:\t\t" + headerType(header->type) + "\n";
    result += "Назначение пакета:\t" + headerAssignment(header->assignment) + "\n";
    result += "Флаги:\t\t" + QString::number(header->flags) + "\n";
    result += "Размер данных:\t" + QString::number(header->dataSize) + "\n";
    result += "Контрольная сумма:\t" + QString::number(headerCheckSum(header)) + "\n";

    ui->convertText->appendPlainText(result);
}

void MainWindow::printIdentificationData(const Identification *body)
{
    Q_ASSERT(body != nullptr);
    QString result;

    result += "\tID:\t\t" + QString::number(body->id) + "\n";
    result += "\tДлина имени:\t" + QString::number(body->nameSize) + "\n";
    result += "\tДлина описания:\t" + QString::number(body->descriptionSize) + "\n";
    result += "\tКаналы записи:\t" + QString::number(body->numberWriteChannel) + "\n";
    result += "\tКаналы чтения:\t" + QString::number(body->numberReadChannel) + "\n";
    result += "\tФлаги:\t\t" + QString::number(body->flags) + "\n";
    result += "\tКонтрольная сумма:\t" + QString::number(identificationCheckSum(body)) + "\n";
    result += "\tИмя устройства:\t" + QByteArray{body->name, body->nameSize} + "\n";
    result += "\tОписание устройства:\t" + QByteArray{body->description, body->descriptionSize} + "\n";

    result += "\tКаналы записи:\n";
    for (int i = 0; i < body->numberWriteChannel; ++i)
    {
        Raw raw(static_cast<Raw::DATA_TYPE>(body->writeChannelType[i]));
        result += "\t\tКанал " + QString::number(i) + ":\t" + raw.strData().second + "\n";
    }

    result += "\tКаналы чтения:\n";
    for (int i = 0; i < body->numberReadChannel; ++i)
    {
        Raw raw(static_cast<Raw::DATA_TYPE>(body->readChannelType[i]));
        result += "\t\tКанал " + QString::number(i) + ":\t" + raw.strData().second + "\n";
    }

    ui->convertText->appendPlainText(result);
}

void MainWindow::printReadWriteData(const Read_Write *body)
{
    Q_ASSERT(body != nullptr);
    QString result;

    result += "\tДлина имени:\t" + QString::number(body->nameSize) + "\n";
    result += "\tНомер канала:\t" + QString::number(body->channelNumber) + "\n";
    result += "\tФлаги:\t\t" + QString::number(body->flags) + "\n";
    result += "\tРазмер данных\t" + QString::number(body->dataSize) + "\n";
    result += "\tКонтрольная сумма:\t" + QString::number(readWriteCheckSum(body)) + "\n";
    result += "\tИмя устройства:\t" + QByteArray{body->name, body->nameSize} + "\n";
    result += "\tДанные:\t" + QByteArray{(const char*)body->data, static_cast<int>(body->dataSize)}.toHex(':') + "\n";

    ui->convertText->appendPlainText(result);
}

void MainWindow::printStateData(const State *body)
{
    Q_ASSERT(body != nullptr);
    QString result;

    result += "\tДлина имени:\t" + QString::number(body->nameSize) + "\n";
    result += "\tСостояние устройства:\t" + staetType(body->state) + "\n";
    result += "\tФлаги:\t\t" + QString::number(body->flags) + "\n";
    result += "\tРазмер данных\t" + QString::number(body->dataSize) + "\n";
    result += "\tКонтрольная сумма:\t" + QString::number(stateCheckSum(body)) + "\n";
    result += "\tИмя устройства:\t" + QByteArray{body->name, body->nameSize} + "\n";
    result += "\tДанные:\t" + QByteArray{(const char*)body->data, static_cast<int>(body->dataSize)}.toHex(':') + "\n";

    ui->convertText->appendPlainText(result);
}

QString MainWindow::headerType(Header::Header_TYPE type) const
{
    switch(type)
    {
    case Header::HEADER_TYPE_REQUEST:
        return "Запрос";
    case Header::HEADER_TYPE_RESPONSE:
        return "Ответ";
    default:
        return "NONE";
    }
}

QString MainWindow::headerAssignment(Header::Header_ASSIGNMENT assignment) const
{
    switch(assignment)
    {
    case Header::HEADER_ASSIGNMENT_IDENTIFICATION:
        return "Идентификация";
    case Header::HEADER_ASSIGNMENT_STATE:
        return "Состояние";
    case Header::HEADER_ASSIGNMENT_READ:
        return "Чтение";
    case Header::HEADER_ASSIGNMENT_WRITE:
        return "Запись";
    case Header::HEADER_ASSIGNMENT_PING_PONG:
        return "Пинг";
    default:
        return "NONE";
    }
}

QString MainWindow::staetType(State::State_STATE state) const
{
    switch(state)
    {
    case State::State_STATE_ONLINE:
        return "Online";
    default:
        return "Offline";
    }
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
    }
    ui->convertText->setPlainText(result);
    auto dataSize = rawData.size();

    int counterHeader = 0, counterIdentification = 0, counterState = 0, counterReadWrite = 0;
    while (rawData.size() > 0)
    {
        bool error;
        uint64_t expectedDataSize = 0, cutDataSize = 0;

        auto header = createPkgs((uint8_t *)rawData.toStdString().c_str(), rawData.size(), &error,
                                 &expectedDataSize, &cutDataSize);
        if (error)
        {
            ui->convertText->appendPlainText("Ошибка данных");
            break;
        }

        if (header == nullptr)
        {
            if (error)
            {
                ui->convertText->appendPlainText("Ошибка конверитрования!");
                return;
            }
            else if(expectedDataSize > 0)
            {
                ui->convertText->appendPlainText("Не полные данные");
                break;
            }
        }

        ui->convertText->appendPlainText("HEADER\n");
        counterHeader++;
        printHeaderData(header);

        if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
        {
            ui->convertText->appendPlainText("\tIDENTIFICATION\n");
            counterIdentification++;
            printIdentificationData(header->identification);
        }
        else if(header->assignment == Header::HEADER_ASSIGNMENT_READ || header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
        {
            ui->convertText->appendPlainText("\tREAD_WRITE\n");
            counterReadWrite++;
            printReadWriteData(header->readWrite);
        }
        else if(header->assignment == Header::HEADER_ASSIGNMENT_STATE)
        {
            ui->convertText->appendPlainText("\tSTATE\n");
            counterState++;
            printStateData(header->state);
        }

        clearHeader(header);

        rawData = rawData.mid(cutDataSize);
        if (cutDataSize == 0)
            break;
    }

    QString msg = QString("Общее количество %1 байт. Header: %2, Ident: %3, ReadWrite: %4, State: %5")
            .arg(dataSize).arg(counterHeader).arg(counterIdentification).arg(counterReadWrite).arg(counterState);

    this->statusBar()->showMessage(msg);
}
