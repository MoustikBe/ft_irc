#include "../../header/mainHeader.hpp"

void ServerNewConnection(int serverSocket, std::string welcome, std::vector<pollfd> *fdPoll, int *id, User Users)
{
    int clientFd = accept(serverSocket, NULL, NULL);
    std::cout << "Nouvelle connection\n";
    
    Users._user
    send(clientFd, welcome.c_str(), welcome.size(), 0);
    pollfd newClient = {clientFd, POLLIN, 0};
    fdPoll->push_back(newClient);
}

void ServerRequest(std::vector<pollfd> &fdPoll, int *i, std::string timeInfo)
{   
    char buffer[1000]= {0};
    int bytes = recv(fdPoll[*i].fd, buffer, sizeof(buffer), 0);
    std::string ServerMsg(buffer);

    if(ServerMsg.substr(0, 4) == "time")
        send(fdPoll[*i].fd, timeInfo.c_str(), timeInfo.size(), 0); 
    else if(ServerMsg.substr(0, 4) == "NICK")
        std::cout << "NickName scenario";
    else if(ServerMsg.substr(0, 4) == "QUIT" || !bytes)
    {
        close(fdPoll[*i].fd);
        fdPoll.erase(fdPoll.begin() + *i);
        std::cout << "Connection stopped\n";    
        (*i)--;
    }
    std::cout << ServerMsg;   
}

void ServerExchange(int serverSocket, std::string welcome, std::string timeInfo)
{
    int id = 0;
    User user();
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
                    ServerNewConnection(serverSocket, welcome, &fdPoll, &id);
                else
                    ServerRequest(fdPoll, &i, timeInfo);
            }
        }

    }
}


void ServerInit(char **argv)
{
    Server srv(atoi(argv[1]));
    sockaddr_in address;
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_port = htons(srv.getPort());
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(serverSocket, (struct sockaddr*)&address, sizeof(address));
    listen(serverSocket, 10);


    ServerExchange(serverSocket, "001\r\n", "TIME : \r\n");
    close(serverSocket);
}