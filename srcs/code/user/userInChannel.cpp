#include "../../header/mainHeader.hpp"

bool User::getIfUserIsInChannel(std::string channelName, int id)
{
    int i = getUserChannelId(_user[id].InChannel, channelName);
    if(i == -1)
        return false;
    return true;
}

void User::setChanel(std::string ChanelName, int fd)
{
    std::string message = ":" + getUserName(fd) + "!user@localhost PRIVMSG #" + ChanelName + " :has joined the channel\r\n";
    
    _user[fd].InChannel.push_back(ChanelName);
    SendMessageForAllUser(ChanelName, message, fd);
}

void User::removeChannel(std::string channel, int id)
{
    int i = getUserChannelId(_user[id].InChannel, channel);
    std::string fullMsg = ":" + getUserName(id) + " PART #" + channel + "\r\n";

    if(i != -1)
    {
        SendMessageForAllUser(channel, fullMsg, id);
        _user[id].InChannel.erase(_user[id].InChannel.begin() + i);
    }
}
