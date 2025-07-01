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
    _user[fd].InChannel.push_back(ChanelName);
}

bool User::getIfUserIsInChannel(std::string channelName, int id)
{
    for(int i = 0;  i < (int)_user[id].InChannel.size(); i++)
    {
        if(_user[id].InChannel[i].data() == channelName)
            return true;
    }
    return false;
}

bool User::getIfChannelExist(std::string channel)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel)
            return(true);
    }
    return(false);
}
void User::setAdminChannel(int fd, std::string OwnerChannel)
{
    _user[fd].OwnerChannel.push_back(OwnerChannel);
}

void User::removeChannel(std::string channel, int id)
{
    for(int i = 0; i < (int)_user[id].InChannel.size(); i++)
    {
        if(_user[id].InChannel[i].data() == channel)
            _user[id].InChannel.erase(_user[id].InChannel.begin() + i);
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

int User::getChannelIndex(std::string channel)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel)
            return(i);
    }
    return(-1);
}

bool User::getChannelTopicStatus(int index)
{
    return(_userChannel[index].TopicActive);
}

void User::setTopicChannel(std::string Topic, int currentChannel)
{
    _userChannel[currentChannel].Topic = Topic;
    std::cout << "New Topic of channel -> " << _userChannel[currentChannel].Topic << "\n";
}

void User::setInvitationChannel(std::string Invitation, int id)
{
    for(int i = 0; i < (int)_user[id].invitedChannel.size(); i++)
    {
        if(_user[id].invitedChannel[i].data() == Invitation)
            return ;
    }
    _user[id].invitedChannel.push_back(Invitation);
}

bool User::getIfIsOnlyInvitation(std::string channel)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel && _userChannel[i].InviteOnly)
            return(true);        
    }
    return(false);
}

bool User::getIfChannelInvitation(std::string channel, int id)
{
    for(int i = 0; i < (int)_user[id].invitedChannel.size(); i++)
    {
        if(_user[id].invitedChannel[i].data() == channel)
            return true;
    }
    return false;
}

void User::setChangeInvitation(std::string channel)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel)
        {
            if(_userChannel[i].InviteOnly)
                _userChannel[i].InviteOnly = false;
            else
                _userChannel[i].InviteOnly = true;
        }
    }
}

void User::CreateChannel(std::string channel)
{
    channelStruct newChannel = {channel, "", true, "", false, false, 0};
    _userChannel.push_back(newChannel);
}