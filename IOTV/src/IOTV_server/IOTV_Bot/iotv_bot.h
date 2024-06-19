#pragma once

#include "qtimer.h"
#include <QObject>
#include <tgbot/tgbot.h>

#include <set>

class IOTV_Bot : public QObject
{
    Q_OBJECT
public:
    IOTV_Bot(const QString &token, const std::set<int64_t> &clients, QObject *parent = nullptr);
    ~IOTV_Bot();

private:
    void startBot();
    bool trustClient(const TgBot::Message::Ptr &message);
    void sendMessage(int64_t id, const QString &message);

    TgBot::Bot _bot;
    std::shared_ptr<TgBot::TgLongPoll> _longPoll;
    std::set<int64_t> _clients;
    std::shared_ptr<QTimer> _timer;

signals:
    void signalStart();
    void signalSendMsgForAll(QString);
    void signalSendMsg(int64_t id, QString);

    void signalBotRequest(int64_t id, QString);

    void signalStop();

private slots:
    void slotSendMessageForAll(QString message);
    void slotTimer();
};
