#include "../../header/mainHeader.hpp"


void ServerExchange(int serverSocket, std::string welcome, std::string timeInfo)
{
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
                {
                    int clientFd = accept(serverSocket, NULL, NULL);
                    std::cout << "Nouvelle connection\n";
                    send(clientFd, welcome.c_str(), welcome.size(), 0);
                    pollfd newClient = {clientFd, POLLIN, 0};
                    fdPoll.push_back(newClient);
                }
            
                else
                {
                    char buffer[1000]= {0};
                    int bytes = recv(fdPoll[i].fd, buffer, sizeof(buffer), 0);
                    std::string ServerMsg(buffer);
                    if(ServerMsg.substr(0, 4) == "time")
                        send(fdPoll[i].fd, timeInfo.c_str(), timeInfo.size(), 0); 
                    else if(ServerMsg.substr(0, 4) == "NICK")
                        std::cout << "NickName scenario"; /* Du coup c'est comme ca qu'on gere les user*/
                    else if(ServerMsg.substr(0, 4) == "QUIT" || !bytes)
                    {
                        close(fdPoll[i].fd);
                        fdPoll.erase(fdPoll.begin() + i);
                        std::cout << "Connection stopped\n";    
                        i--;
                    }
                    std::cout << ServerMsg;
                }
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
    std::string welcome = "001\r\n";
    std::string timeInfo = "TIME : \r\n";
    ServerExchange(serverSocket, "001\r\n", "TIME : \r\n");
    close(serverSocket);
}