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

void requestChangeName(int fd, std::string ServerMsg, User *Users)
{
    std::string newName = ServerMsg.substr(5, ServerMsg.size() - 7);
    Users->getNameValidity(fd, newName, &userData::name, &userData::hasProvideNickName);
}

bool IsChanel(std::string message)
{
    for(int i = 0; message[i] != ':'; i++)
        if(message[i] == '#')
            return(true);
    return(false);
}

void requestChanelMessage(User *Users, std::string message, int fd)
{
    int start = message.find('#');
    int end = message.find(':');
    std::string chanelName = message.substr(start + 1, end - start - 2);
    std::string UserMsg = message.substr(end + 1, message.size() - end - 3);
    if(!Users->getIfUserIsInChannel(chanelName, fd))
        return ; // Return un message d'erreur au serveur.
    for(int i = 0; i < Users->getLen(); i++)
    {
        if(Users->getIfUserIsInChannel(chanelName, i) && i != fd)
        {
            std::string fullMsg = ":" + Users->getUserName(fd) + "!user@localhost PRIVMSG #" + chanelName + " :" + UserMsg + "\r\n";
            std::cout << "Sending: [" << fullMsg << "] to fd " << Users->getUserFd(i) << std::endl;
            send(Users->getUserFd(i), fullMsg.c_str(), fullMsg.size(), 0);
        } 
    }
    return ;
}

void requestJoin(User *Users, std::string JoinMsg, int fd)
{
    std::istringstream iss(JoinMsg);
    std::string command, ChanelName, password;
    iss >> command >> ChanelName;

    if(ChanelName[0] == '#')
        ChanelName = ChanelName.substr(1);
    int flag = 0;
    if(Users->getIfRequirePassword(ChanelName))
    {
        iss >> password;
        if(!Users->checkIfPasswordValid(password, ChanelName))
        {
            Users->SendNotification(fd, "#" + ChanelName + " this channel need a valid password.\r\n");
            return ;
        }
    }
    if(Users->getIfIsOnlyInvitation(ChanelName))
    {
        std::cout << "Channel trouver, et est en invite only\n";
        if(Users->getIfChannelInvitation(ChanelName, fd))
        {
            if(Users->getIfChannelNotFull(ChanelName))
                Users->setChanel(ChanelName, fd);
            else
                Users->SendNotification(fd, "#" + ChanelName + " Channel is full\r\n");
        }
        else
        {
            Users->SendNotification(fd, "#" + ChanelName + " this channel is in invitation only and you didn't get invited.\r\n");
            std::cout << "Tu n'as le channel dans la liste de channel qui ton inviter\n";
        }
    }
    else
    {
        if(Users->getIfChannelExist(ChanelName))
            flag = 1; 
        if(!flag)
        {
            std::cout << "L'utilisateur : " << Users->getUserName(fd) << " est devenus admin de " << ChanelName << "\n";
            Users->SendNotification(fd, "#" + ChanelName + " New channel created. You are the admin of the channel. \r\n");
            Users->setAdminChannel(fd, ChanelName);
            Users->CreateChannel(ChanelName);
        }
        if(Users->getIfChannelNotFull(ChanelName))
        {
            std::string message = ":" + Users->getUserName(fd) + "!user@localhost PRIVMSG #" + ChanelName + " :has joined the channel\r\n";
            std::cout << "adding the channnel";
            Users->setChanel(ChanelName, fd);
        }
        else
            Users->SendNotification(fd, "#" + ChanelName + " Channel is full\r\n");
    }
}

void requestMessage(User *Users, std::string message, int fd)
{
    bool UserFound = false;
    std::string SendTo;
    int index = message.find(":");
    if(IsChanel(message))
    {
        requestChanelMessage(Users, message, fd);
        return;
    }
    if(index)
        SendTo = message.substr(index + 1, message.size() - index - 3);
    for(int i = 0; i < Users->getLen(); i++)
    {
        if(Users->getUserName(i) == SendTo)
        {
            std::string You = "You";
            std::string MessageFrom = ":" + You + "!user@localhost PRIVMSG #" + message.substr(7, index - 7) + "\r\n";
            std::string MessageTo = ":" + Users->getUserName(fd) + "!user@localhost PRIVMSG #" + message.substr(7, index - 7) + "\r\n";
            send(fd, MessageFrom.c_str(), MessageFrom.size(), 0);
            send(Users->getUserFd(i), MessageTo.c_str(), MessageTo.size(), 0);
            UserFound = true;
        } 
    }
    if(!UserFound)
        Users->SendNotification(fd, "WARNING User not found. \r\n");
}

void requestTopic(User *Users, std::string message, int fd)
{
    std::istringstream iss(message);
    std::string command, channel, Topic;
    iss >> command >> channel >> Topic;

    if(channel[0] == '#')
        channel = channel.substr(1);
    if(Topic[0] == ':')
        Topic = Topic.substr(1);
    int currentChannel = Users->getChannelIndex(channel);
    std::cout << "current -> " << currentChannel << "\nChannel " << channel; 
    if(currentChannel != -1)
    {
        if(Users->getChannelTopicStatus(currentChannel))
        {
            if(Users->getPrivilege(channel, fd))
            {
                std::string commandToSend = "TOPIC #" + channel + " :" + Topic + "\r\n";
                Users->SendMessageForAllUser(channel, commandToSend, -1);
                //send(fd, commandToSend.c_str(), commandToSend.length(), 0);
                Users->setTopicChannel(Topic, currentChannel);
            }
            else 
                Users->SendNotification(fd, "#" + channel + " You must be admin of the channel to do that. \r\n");
        }
        else
        {
            std::string commandToSend = "TOPIC #" + channel + " :" + Topic + "\r\n";
            Users->SendMessageForAllUser(channel, commandToSend, -1);
            //send(fd, commandToSend.c_str(), commandToSend.length(), 0);
            Users->setTopicChannel(Topic, currentChannel);
        }
    }
}

void requestKick(User *Users, std::string message, int fd)
{
    std::istringstream iss(message);
    std::string command, channel, nameToKick;
    iss >> command >> channel >> nameToKick;
    if(channel[0] == '#')
        channel = channel.substr(1);
    if(!Users->getPrivilege(channel, fd))
    {
        std::cout << "No perm\n";
        return ; // Should output error message
    }
    for(int i = 0; i < Users->getLen(); i++)
    {
        if(Users->getUserName(i) == nameToKick && Users->getIfUserIsInChannel(channel, i))
        {
            std::string notify = ":" + Users->getUserName(fd) + " KICK #" + channel + " " + nameToKick + " :Kicked from channel\r\n";
            Users->SendMessageForAllUser(channel, notify, -1);
            Users->removeChannel(channel, i);
        }
    }
    std::cout << "Command : " << command << "\nChanel : " << channel << "\nnameToKick : " << nameToKick << "\n"; 
}

void requestInvite(User *Users, std::string message, int fd)
{
    std::istringstream iss(message);
    std::string command, nameToInvite, channel;
    iss >> command >> nameToInvite >> channel;

    if(channel[0] == '#')
        channel = channel.substr(1);
    for(int i = 0; i < Users->getLen(); i++)
    {
        if(Users->getUserName(i) == nameToInvite)
        {
            std::string notify = ":" + Users->getUserName(fd) + "!user@localhost NOTICE WARNING " + Users->getUserName(fd) + " Invite you to the channel " + channel + "\r\n";
            send(i, notify.c_str(), notify.length(), 0);
            Users->setInvitationChannel(channel, i);
        }
    }
}

void  requestMode(User *Users, std::string message, int fd)
{
    std::istringstream iss(message);
    std::string command, channel, flag;
    iss >> command >> channel >> flag;

    if(channel[0] == '#')
        channel = channel.substr(1);
    if(flag == "-i" || flag == "+i")
    {
        
        std::string notify = ":" + Users->getUserName(fd) + "!user@localhost NOTICE #" + channel;
        if(flag == "+i")
        {
            notify += " in invitation only\r\n";
            Users->setBoolReverse(channel, &channelStruct::InviteOnly, true, notify);
        }
        else
        { 
            notify += " free to join\r\n";
            Users->setBoolReverse(channel, &channelStruct::InviteOnly, false, notify);
        }
    }
    else if(flag == "-t" || flag == "+t")
    {
        std::string notify = ":" + Users->getUserName(fd) + "!user@localhost NOTICE #" + channel;
        if(flag == "+t")
        {
            notify += " topic can be only changed by admin\r\n";
            Users->setBoolReverse(channel, &channelStruct::TopicActive, true, notify);
        }
        else
        {
            notify += " topic can be changed by everyone\r\n";
            Users->setBoolReverse(channel, &channelStruct::TopicActive, false, notify);
        }
    }
    else if(flag == "-o" || flag == "+o")
    {
        std::string UserName;
        iss >> UserName;

        int id = Users->getUserIdByName(UserName);
        if(id == -1)
            return; // User not found, error //
        std::string notify = ":" + Users->getUserName(fd) + "!user@localhost NOTICE #" + channel + " the user : " + Users->getUserName(id);
        if(flag == "+o")
        {
            notify += " is an admin now\r\n";
            Users->setAdminChannel(id, channel);
        }
        else 
        {
            notify += "  is no more an admin\r\n";
            Users->unsetAdminChannel(id, channel);
        }
        Users->SendMessageForAllUser(channel, notify, -1);
    }
    else if(flag == "-l" || flag == "+l")
    {
        std::string notify = ":" + Users->getUserName(fd) + "!user@localhost NOTICE #" + channel;
        if(flag == "-l")
        {
            notify += " is with unlimited amount of users\r\n";
            Users->setLimitChannel(channel, -1);
        }
        else
        {
            std::string limit;
            iss >> limit;
            notify += " has a restriction of " + limit + " of user(s)\r\n";
            Users->setLimitChannel(channel, atoi(limit.c_str()));
        }
        Users->SendMessageForAllUser(channel, notify, -1);
    }
    else if(flag == "-k" || flag == "+k")
    {
        std::string password;
        iss >> password;

        std::string notify = ":" + Users->getUserName(fd) + "!user@localhost NOTICE #" + channel;
        if(flag == "-k")
        {
            notify += " don't need a password for a user his able to connect\r\n";
            Users->setBoolReverse(channel, &channelStruct::passwordActive, false, notify);
        }
        else
        {
            notify += " need a password for a user to connect\r\n";
            Users->setBoolReverse(channel, &channelStruct::passwordActive, true, notify);
            Users->setPassword(channel, password);
        }
    }
}

void    requestPart(User *Users, std::string ServerMsg, int fd)
{
    std::istringstream iss(ServerMsg);
    std::string command, channel, flag;
    iss >> command >> channel;

    if(channel[0] == '#')
        channel = channel.substr(1);
    Users->removeChannel(channel, fd);
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
