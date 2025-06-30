#include "../../header/mainHeader.hpp"

void User::setUsername(std::string name, int fd)
{
    std::cout << "Username " << name << " Set on the user[fd] " << fd << "\n"; 
    _user[fd].name = name;
} 

void User::setSocket(int fd)
{
    _user[fd].socketUser = fd;
}

void User::getAllDataUser()
{
    std::cout << "--------------\n";
    for(int i = 0; i < (int)_user.size(); i++)
    {
        if(_user[i].name != "")
            std::cout << _user[i].name << "\n";
    }
    std::cout << "--------------\n";
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

int User::getUserFd(int id)
{
    return(_user[id].socketUser);
}

void User::setChanel(std::string ChanelName, int fd)
{
    channelStruct newChannel= {ChanelName, "", false, "", false, 0};
    _user[fd].channel.push_back(newChannel);
}

bool User::getIfChannelExist(std::string channelName, int id)
{
    for(int i = 0;  i < (int)_user[id].channel.size(); i++)
    {
        if(_user[id].channel[i].channelName.data() == channelName)
            return true;
    }
    return false;
}

void User::setAdminChannel(int fd, std::string OwnerChannel)
{
    _user[fd].OwnerChannel.push_back(OwnerChannel);
}

void User::removeChannel(std::string channel, int id)
{
    for(int i = 0; i < (int)_user[id].channel.size(); i++)
    {
        if(_user[id].channel[i].channelName.data() == channel)
            _user[id].channel.erase(_user[id].channel.begin() + i);
    }
}

bool User::getPrivilege(std::string channel, int id)
{
    for(int i = 0;  i < (int)_user[id].OwnerChannel.size(); i++)
    {
        if(_user[id].OwnerChannel[i].data() == channel)
            return true;
    }
    return false;
}

int User::getChannelIndex(std::string channel, int id)
{
    for(int i = 0; i < (int)_user[id].channel.size(); i++)
    {
        if(_user[id].channel[i].channelName.data() == channel)
            return(i);
    }
    return(-1);
}

bool User::getChannelTopicStatus(int id, int index)
{
    return(_user[id].channel[index].TopicAdmin);
}

void User::setTopicChannel(std::string Topic, int id, int currentChannel)
{
    _user[id].channel[currentChannel].Topic = Topic;
    std::cout << "New Topic of channel -> " << _user[id].channel[currentChannel].Topic << "\n";
}