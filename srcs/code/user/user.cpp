#include "../../header/mainHeader.hpp"

int User::getUserChannelId(const std::vector<std::string>& channels, const std::string& channelName) 
{
    for (size_t i = 0; i < channels.size(); ++i) 
    {
        if (channels[i] == channelName)
            return i;
    }
    return -1;
}

void User::SendNotification(int id, std::string message)
{
    std::string notify = ":" + getUserName(id) + "!user@localhost NOTICE " + message;
    send(id, notify.c_str(), notify.size(), 0);
}

void User::SendMessageForAllUser(std::string channel, std::string message, int id)
{
    for (size_t i = 0; i < _user.size(); ++i)
    {
        for(size_t j = 0; j < _user[i].InChannel.size(); ++j)
        {
            if(_user[i].InChannel[j].data() == channel && (int)i != id)
                send(i, message.c_str(), message.size(), 0);
        }
    }
    return;
}
