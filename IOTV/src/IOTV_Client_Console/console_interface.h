#pragma once

#include "client.h"

class Console_Interface
{
public:
    Console_Interface(Client &client);

    void exec();

private:
    Client &_client;

    void printScreen() const;
    void inputCommand();
};

