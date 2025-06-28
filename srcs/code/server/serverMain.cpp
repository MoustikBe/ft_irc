#include "../../header/mainHeader.hpp"

void ServerNewConnection(int serverSocket, std::string welcome, std::vector<pollfd> *fdPoll, int *id, User *Users)
{
    int clientFd = accept(serverSocket, NULL, NULL);
    std::cout << "Nouvelle connection\n";
    
    Users->setSocket(clientFd);
    send(clientFd, welcome.c_str(), welcome.size(), 0);
    pollfd newClient = {clientFd, POLLIN, 0};
    fdPoll->push_back(newClient);
    (*id)++;
}

int detectNewLine(std::string ServerMsg)
{
    int index = 0;

    for(int i = 0; ServerMsg[i] != '\n'; i++)
        index++;
    return(index);
}

void requestChangeName(int fd, std::string ServerMsg, User *Users)
{
    std::string newName = ServerMsg.substr(5, ServerMsg.size() - 7);
    Users->setUsername(newName, fd);
}

void requestMessage(User *Users, std::string message, int fd)
{
    std::string SendTo;
    int index = message.find(":");
    if(index)
        SendTo = message.substr(index + 1, message.size() - index - 3);
    for(int i = 0; i < Users->getLen(); i++)
    {
        if(Users->getUserName(i) == SendTo)
        {
            std::cout <<  Users->getUserName(fd) << " test\n";
            std::string MessageFrom = "\033[0;31mYou:" + message.substr(7, index - 7) + "\r\n";
            std::string MessageTo = "\033[0;32m" + Users->getUserName(fd) + ":" + message.substr(7, index - 7) + "\r\n";
            send(fd, MessageFrom.c_str(), MessageFrom.size(), 0);
            send(Users->getUserFd(i), MessageTo.c_str(), MessageTo.size(), 0);
        } 
    }
}

void ServerRequest(std::vector<pollfd> &fdPoll, int *i, User *Users)
{   
    char buffer[1000]= {0};
    int bytes = recv(fdPoll[*i].fd, buffer, sizeof(buffer), 0);
    std::string ServerMsg(buffer);

    if(ServerMsg.substr(0, 4) == "info")
        Users->getAllDataUser();
    else if(ServerMsg.substr(0, 4) == "NICK")
        requestChangeName(fdPoll[*i].fd, ServerMsg, Users);
    else if(ServerMsg.substr(0, 7) == "PRIVMSG")
        requestMessage(Users, ServerMsg, fdPoll[*i].fd);
    else if(ServerMsg.substr(0, 4) == "QUIT" || !bytes)
    {
        close(fdPoll[*i].fd);
        fdPoll.erase(fdPoll.begin() + *i);
        std::cout << "Connection stopped\n";    
        (*i)--;
    }
    std::cout << ServerMsg;
}

void ServerExchange(int serverSocket, std::string welcome)
{
    int id = 0;
    User Users;
    std::vector<pollfd> fdPoll;
    pollfd serverPollFd = {serverSocket, POLLIN, 0};

    fdPoll.push_back(serverPollFd);
    while(true)
    {
        int pol = poll(fdPoll.data(), fdPoll.size(), 0);
        if(pol == -1)
        {
            error("poll");
            exit(1);
        }
        for(int i = 0; i < (int)fdPoll.size(); i++)
        {
            if(fdPoll[i].revents & POLLIN)
            {
                if(fdPoll[i].fd == serverSocket)
                    ServerNewConnection(serverSocket, welcome, &fdPoll, &id, &Users);
                else
                    ServerRequest(fdPoll, &i, &Users);
            }
        }

    }
}

void ServerData(sockaddr_in *address, Server srv)
{
    address->sin_family = AF_INET;
    address->sin_port = htons(srv.getPort());
    address->sin_addr.s_addr = inet_addr("127.0.0.1");
}


void ServerInit(char **argv)
{
    Server srv(atoi(argv[1]));
    sockaddr_in address;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ServerData(&address, srv);
    bind(serverSocket, (struct sockaddr*)&address, sizeof(address));
    listen(serverSocket, 10);
    ServerExchange(serverSocket, "001\r\n");
    close(serverSocket);
}