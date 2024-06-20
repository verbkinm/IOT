#include "iotv_bot.h"

#include "log.h"
#include "ConfigTypes.h"
#include "qthread.h"
#include <QDebug>

std::shared_ptr<std::vector<std::string>> allowUpdates = std::make_shared<std::vector<std::string>>();
//allowUpdates->push_back("[\"message\"]");
    ;
IOTV_Bot::IOTV_Bot(const QString &token, const std::set<int64_t> &clients, QObject *parent)
    : QObject{parent}, _bot(token.toStdString()),/* _longPoll(_bot, 1, 1),*/ _clients(clients)
{
    connect(this, &IOTV_Bot::signalStart, this, &IOTV_Bot::startBot, Qt::QueuedConnection);
    connect(this, &IOTV_Bot::signalSendMsg, this, &IOTV_Bot::sendMessage, Qt::QueuedConnection);
    connect(this, &IOTV_Bot::signalSendMsgForAll, this, &IOTV_Bot::slotSendMessageForAll, Qt::QueuedConnection);
}

IOTV_Bot::~IOTV_Bot()
{
    qDebug() << "IOTV_Bot destruct";
    QThread *thread = QThread::currentThread();
    _timer->moveToThread(thread);
}

bool IOTV_Bot::trustClient(const TgBot::Message::Ptr &message)
{
    auto id = message->chat->id;
    if (!_clients.contains(id))
    {
        sendMessage(id, QString(message->chat->username.c_str())
                            + " ("
                            + QString::number(id)
                            + ") - Я тебя не знаю! Не пиши мне больше!");
        return false;
    }
    return true;
}

void IOTV_Bot::sendMessage(int64_t id, const QString &message)
{
    try
    {
        _bot.getApi().sendMessage(id, message.toStdString());
        auto get = _bot.getApi().getChat(id);

        QString msg = "id: "
                      + QString::number(id)
                      + ", username: \""
                      + get->username.c_str()
                      + "\" -> \""
                      + message
                      + "\"";

        Log::write(CATEGORY::TGBOT, msg, Log::Write_Flag::FILE_STDOUT, tgBotFileName::TGBOT_LOG_FILENAME);
    }
    catch (...)
    {
        //
    }

}

void IOTV_Bot::startBot()
{
    allowUpdates->push_back("[\"message\"]");
    _longPoll = std::make_shared<TgBot::TgLongPoll>(_bot, 1, 1, allowUpdates);
    slotSendMessageForAll("Питание включено.");

    _timer.reset();
    _timer = std::make_shared<QTimer>();
    connect(_timer.get(), &QTimer::timeout, this, &IOTV_Bot::slotTimer);

    _timer->start(10);



    _bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr message){
        if (!trustClient(message))
            return;

        QString msg = "id: "
                      + QString::number(message->chat->id)
                      + ", username: \""
                      + QString(message->chat->username.c_str())
                      + "\" <- \""
                      + QString(message->text.c_str())
                      + "\"";
        Log::write(CATEGORY::TGBOT, msg, Log::Write_Flag::FILE_STDOUT, tgBotFileName::TGBOT_LOG_FILENAME);
    });

    _bot.getEvents().onCommand("clients", [&](TgBot::Message::Ptr message){
        if (!trustClient(message))
            return;

        emit signalBotRequest(message->chat->id, "clients");
    });

    _bot.getEvents().onCommand("hosts", [&](TgBot::Message::Ptr message){
        if (!trustClient(message))
            return;

        emit signalBotRequest(message->chat->id, "hosts");
    });

    _bot.getEvents().onCommand("help", [&](TgBot::Message::Ptr message){
        if (!trustClient(message))
            return;

        sendMessage(message->chat->id, "Список команд:\n"
                                                       "/clients - список подключенных клиентов\n"
                                                       "/hosts - список подключенных хостов\n"
                                                       "/help - вывод справки");
    });
}

void IOTV_Bot::slotSendMessageForAll(QString message)
{
    for (const auto &client : _clients)
        sendMessage(client, message);
}

void IOTV_Bot::slotTimer()
{
    _timer->stop();
    try
    {
        _longPoll->start();
    }
    catch (TgBot::TgException& e)
    {
        printf("error: %s\n", e.what());
    }
    catch (...)
    {
        std::cerr << "exeption\n";
    }

    _timer->start();
}
