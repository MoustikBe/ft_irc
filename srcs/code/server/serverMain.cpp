#include "../../header/mainHeader.hpp"

void ServerNewConnection(int serverSocket, std::vector<pollfd> *fdPoll, int *id, User *Users)
{
    int clientFd = accept(serverSocket, NULL, NULL);
    if (clientFd == -1) 
    {
        perror("accept");
        return;
    }
    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags == -1) 
    {
        perror("fcntl F_GETFL");
        close(clientFd);
        return;
    }
    if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1) 
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

int detectNewLine(std::string ServerMsg)
{
    int index = 0;

    for(int i = 0; ServerMsg[i] != '\n'; i++)
        index++;
    return(index);
}

bool IsChanel(std::string message)
{
    for(int i = 0; message[i] != ':'; i++)
        if(message[i] == '#')
            return(true);
    return(false);
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
    Users->removeACredit(id); // Faudrait implementer un message d'erreur mais dans tout les cas c'est foutus pour lui sans le mdp //
}

void ServerRequest(std::vector<pollfd> &fdPoll, int *i, User *Users, Server *srv)
{   
    char buffer[1000]= {0};
    int bytes = recv(fdPoll[*i].fd, buffer, sizeof(buffer), 0);
    std::string ServerMsg(buffer);

    if(Users->getIfhasHabilitation(fdPoll[*i].fd))
    {
        if(ServerMsg.substr(0, 4) == "info")
            Users->getAllDataUser();
        else if(ServerMsg.substr(0, 4) == "NICK")
            requestChangeName(fdPoll[*i].fd, ServerMsg, Users);
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
