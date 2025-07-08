#include "../../header/mainHeader.hpp"

bool User::getChannelTopicStatus(int index)
{
    return(_userChannel[index].TopicActive);
}

int User::getUserFd(int id)
{
    return(_user[id].socketUser);
}

std::string User::getUserName(int fd)
{
    return(_user[fd].name);
}

int User::getLen()
{
    int count = 0; 
    for(int i = 0; i < (int)_user.size(); i++)
        count++;
    return(count);
}

void User::getAllUserChannel(int id, const std::string& header, const std::vector<std::string>& channels)
{
    std::string message = header + "\r\n";
    for (size_t i = 0; i < channels.size(); ++i)
    {
        message += "- " + channels[i] + "\r\n";
    }
    send(id, message.c_str(), message.size(), 0);
}

void User::getAllDataUser(int id)
{
    std::string allInfo =
        ":" + getUserName(id) + "!user@localhost NOTICE #" + "INFO :\r\n"
        "---------- BOT INFO ----------\r\n"
        "UserName : " + _user[id].username + "\r\n"
        "NickName : " + _user[id].name + "\r\n";
    send(id, allInfo.c_str(), allInfo.size(), 0);

    getAllUserChannel(id, "\nChannels you are in :", _user[id].InChannel);
    getAllUserChannel(id, "\nChannels you own :", _user[id].OwnerChannel);
    getAllUserChannel(id, "\nChannels you are invited to :", _user[id].invitedChannel);
}


int User::getUserIdByName(std::string UserName)
{
    for(int i = 0; i < (int)_user.size(); i++)
    {
        if(_user[i].name.data() == UserName)
            return(i);
    }
    return(-1);
}

int User::getCredit(int id)
{
    return(_user[id].credit);
}
