#include "console_interface.h"

Console_Interface::Console_Interface(Client &client) : _client{client}
{
    _client.connectToHost();
}

void Console_Interface::exec()
{
    while(true)
    {
        printScreen();
        inputCommand();
    }
}

void Console_Interface::printScreen() const
{
//    system("clear");
    std::cout << "IOTV_Console_Client\n"
              << "Connection state: ";
    if (_client.connectionState() == QAbstractSocket::ConnectedState)
        std::cout << "conected\n";
    else
        std::cout << "disconnected\n";


}

void Console_Interface::inputCommand()
{
    char command;
    std::cin >> command;

    switch (command)
    {
        case 'c':
        _client.connectToHost();
    }

}
