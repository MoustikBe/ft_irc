#include "../../header/mainHeader.hpp"

bool IsChanel(std::string message)
{
    for(int i = 0; message[i] != ':'; i++)
        if(message[i] == '#')
            return(true);
    return(false);
}

void ServerRequest(std::vector<pollfd> &fdPoll, int *i, User *Users, Server *srv)
{   
    std::vector<char> buffer(1000);
    int bytes = recv(fdPoll[*i].fd, buffer.data(), buffer.size(), 0);
    std::string ServerMsg(buffer.data(), bytes);

    if(Users->getIfhasHabilitation(fdPoll[*i].fd))
    {
        if(ServerMsg.substr(0, 4) == "info")
            Users->getAllDataUser(fdPoll[*i].fd);
        else if(ServerMsg.substr(0, 4) == "NICK")
            requestChangeName(fdPoll[*i].fd, ServerMsg, Users);
        else if(ServerMsg.substr(0, 8) == "userhost")
            requestChangeUserName(fdPoll[*i].fd, ServerMsg, Users);
        else if(ServerMsg.substr(0, 7) == "PRIVMSG")
            requestMessage(Users, ServerMsg, fdPoll[*i].fd);
        else if(ServerMsg.substr(0, 4) == "JOIN")
            requestJoin(Users, ServerMsg, fdPoll[*i].fd);
        else if(ServerMsg.substr(0, 4) == "KICK")
            requestKick(Users, ServerMsg, fdPoll[*i].fd);
        else if(ServerMsg.substr(0, 5) == "TOPIC")
            requestTopic(Users, ServerMsg, fdPoll[*i].fd);
        else if(ServerMsg.substr(0, 6) == "INVITE")
            requestInvite(Users, ServerMsg, fdPoll[*i].fd);
        else if(ServerMsg.substr(0, 4) == "MODE")
            requestMode(Users, ServerMsg, fdPoll[*i].fd);
        else if(ServerMsg.substr(0, 4) == "PART")
            requestPart(Users, ServerMsg, fdPoll[*i].fd);
        else if(ServerMsg.substr(0, 4) == "QUIT" || !bytes)
        {
            close(fdPoll[*i].fd);
            fdPoll.erase(fdPoll.begin() + *i);
            std::cout << "Connection stopped\n";    
            (*i)--;
        }
        std::cout << ServerMsg;
    }
    else if(Users->getCredit(fdPoll[*i].fd))
        requestAuthentification(ServerMsg, Users, srv, fdPoll[*i].fd);
    else
    {
        Users->SendNotification(fdPoll[*i].fd, "WARNING Wrong password\r\n");
        close(fdPoll[*i].fd);
        fdPoll.erase(fdPoll.begin() + *i);
        std::cout << "Connection refused, password issue\n";
        (*i)--;
    }
}

void ServerExchange(int serverSocket, Server *srv)
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
                    ServerNewConnection(serverSocket, &fdPoll, &id, &Users);
                else
                    ServerRequest(fdPoll, &i, &Users, srv);
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
    Server srv(atoi(argv[1]), argv[2]);
    sockaddr_in address;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ServerData(&address, srv);
    bind(serverSocket, (struct sockaddr*)&address, sizeof(address));
    listen(serverSocket, 10);
    ServerExchange(serverSocket, &srv);
    close(serverSocket);
}
