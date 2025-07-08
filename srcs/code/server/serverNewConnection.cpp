#include "../../header/mainHeader.hpp"

void ServerNewConnection(int serverSocket, std::vector<pollfd> *fdPoll, int *id, User *Users)
{
    int clientFd = accept(serverSocket, NULL, NULL);
    if (clientFd == -1) 
    {
        perror("accept");
        return;
    }
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL");
        close(clientFd);
        return;
    }    
    std::cout << "Nouvelle connection\n";
    std::string connection = "001\r\n";
    Users->setSocket(clientFd);
    send(clientFd, connection.c_str(), connection.size(), 0);
    pollfd newClient = {clientFd, POLLIN, 0};
    fdPoll->push_back(newClient);
    (*id)++;
}

void requestAuthentification(std::string ServerMsg, User *Users, Server *srv, int id)
{
    std::istringstream stream(ServerMsg);
    std::string line;
    std::vector<std::string> commands;

    while (std::getline(stream, line)) 
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (!line.empty())
            commands.push_back(line);
    }
    for(int k = 0; k < (int)commands.size(); k++)
    {
        std::istringstream iss(commands[k]);
        std::string command, option;
        iss >> command >> option;
        if(commands[k].substr(0, 4) == "PASS")
            Users->getPasswordValidity(id, option, srv);
        if(commands[k].substr(0, 4) == "NICK")
            Users->getNameValidity(id, option, &userData::name, &userData::hasProvideNickName);
        if(commands[k].substr(0, 4) == "USER")
            Users->getNameValidity(id, option, &userData::username, &userData::hasProvideUserName );
        std::cout << command.data() << "\n";
    }
    Users->removeACredit(id);
}
