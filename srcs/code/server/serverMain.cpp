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
    int index = JoinMsg.find('#');
    int flag = 0;

    std::string ChanelName = JoinMsg.substr(index + 1, JoinMsg.size() - index - 3);
    if(Users->getIfIsOnlyInvitation(ChanelName))
    {
        std::cout << "Channel trouver, et est en invite only\n";
        if(Users->getIfChannelInvitation(ChanelName, fd))
        {
            if(Users->getIfChannelNotFull(ChanelName))
                Users->setChanel(ChanelName, fd);
        }
        else
            std::cout << "Tu n'as le channel dans la liste de channel qui ton inviter\n";
    }
    else
    {
        if(Users->getIfChannelExist(ChanelName))
            flag = 1; 
        if(!flag)
        {
            std::cout << "L'utilisateur : " << Users->getUserName(fd) << " est devenus admin de " << ChanelName << "\n";
            Users->setAdminChannel(fd, ChanelName);
            Users->CreateChannel(ChanelName);
        }
        if(Users->getIfChannelNotFull(ChanelName))
        {
            std::cout << "adding the channnel";
            Users->setChanel(ChanelName, fd);
        }
        else 
            std::cout << "C'est full";
    }
}

void requestMessage(User *Users, std::string message, int fd)
{
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
        } 
    }
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
                send(fd, commandToSend.c_str(), commandToSend.length(), 0);
                Users->setTopicChannel(Topic, currentChannel);
            }
        }
        else
        {
            std::string commandToSend = "TOPIC #" + channel + " :" + Topic + "\r\n";
            send(fd, commandToSend.c_str(), commandToSend.length(), 0);
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
            int socket = Users->getUserFd(i);
            std::string notify = ":" + Users->getUserName(fd) + " KICK #" + channel + " " + nameToKick + " :Kicked from channel\r\n";
            send(socket, notify.c_str(), notify.length(), 0);
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
            std::string notify = Users->getUserName(fd) + " invited you to join " + channel + "\r\n";
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
        std::string notify = Users->getUserName(fd) + " invitation only restriction has been changed " + channel + "\r\n";
        send(fd, notify.c_str(), notify.length(), 0);
        if(flag == "+i")
            Users->setBoolReverse(channel, &channelStruct::InviteOnly, true);
        else 
            Users->setBoolReverse(channel, &channelStruct::InviteOnly, false);
    }
    else if(flag == "-t" || flag == "+t")
    {
        std::string notify = Users->getUserName(fd) + " topic only changed by admin has been changed " + channel + "\r\n";
        send(fd, notify.c_str(), notify.length(), 0);
        if(flag == "+t")
            Users->setBoolReverse(channel, &channelStruct::TopicActive, true);
        else
            Users->setBoolReverse(channel, &channelStruct::TopicActive, false);
    }
    else if(flag == "-o" || flag == "+o")
    {
        std::string UserName;
        iss >> UserName;

        int id = Users->getUserIdByName(UserName);
        if(id == -1)
            return; // User not found, error //
        if(flag == "+o")
            Users->setAdminChannel(id, channel);
        else 
            Users->unsetAdminChannel(id, channel);
    }
    else if(flag == "-l" || flag == "+l")
    {
        if(flag == "-l")
            Users->setLimitChannel(channel, -1);
        else
        {
            std::string limit;
            iss >> limit;
            Users->setLimitChannel(channel, atoi(limit.c_str()));
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