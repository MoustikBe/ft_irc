#include "../../header/mainHeader.hpp"

void requestChangeName(int fd, std::string ServerMsg, User *Users)
{
    std::string newName = ServerMsg.substr(5, ServerMsg.size() - 7);
    Users->getNameValidity(fd, newName, &userData::name, &userData::hasProvideNickName);
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
            Users->SendNotification(i, "WARNING" + Users->getUserName(fd) + " Invite you to the channel " + channel + " \r\n");
            Users->setInvitationChannel(channel, i);
        }
    }
}

void requestChanelMessage(User *Users, std::string message, int fd)
{
    int start = message.find('#');
    int end = message.find(':');
    std::string chanelName = message.substr(start + 1, end - start - 2);
    std::string UserMsg = message.substr(end + 1, message.size() - end - 3);
    if(!Users->getIfUserIsInChannel(chanelName, fd))
    {
        Users->SendNotification(fd, "#" + chanelName + " You're not connected to the channel. \r\n");
        return ;
    }
    for(int i = 0; i < Users->getLen(); i++)
    {
        if(Users->getIfUserIsInChannel(chanelName, i) && i != fd)
        {
            std::string fullMsg = ":" + Users->getUserName(fd) + "!user@localhost PRIVMSG #" + chanelName + " :" + UserMsg + "\r\n";
            send(Users->getUserFd(i), fullMsg.c_str(), fullMsg.size(), 0);
        } 
    }
    return ;
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
        if(Users->getIfChannelInvitation(ChanelName, fd))
        {
            if(Users->getIfChannelNotFull(ChanelName))
                Users->setChanel(ChanelName, fd);
            else
                Users->SendNotification(fd, "#" + ChanelName + " Channel is full\r\n");
        }
        else
            Users->SendNotification(fd, "#" + ChanelName + " this channel is in invitation only and you didn't get invited.\r\n");
    }
    else
    {
        if(Users->getIfChannelExist(ChanelName))
            flag = 1; 
        if(!flag)
        {
            Users->SendNotification(fd, "#" + ChanelName + " New channel created. You are the admin of the channel. \r\n");
            Users->setAdminChannel(fd, ChanelName);
            Users->CreateChannel(ChanelName);
        }
        if(Users->getIfChannelNotFull(ChanelName))
            Users->setChanel(ChanelName, fd);
        else
            Users->SendNotification(fd, "#" + ChanelName + " Channel is full\r\n");
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
