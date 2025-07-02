#include "../../header/mainHeader.hpp"

void User::setUsername(std::string name, int fd)
{
    std::cout << "Username " << name << " Set on the user[fd] " << fd << "\n"; 
    _user[fd].name = name;
} 

void User::setSocket(int fd)
{
    _user[fd].credit = 4;
    _user[fd].hasProvidePassword = false;
    _user[fd].hasProvideNickName = false;
    _user[fd].hasProvideUserName = false;
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

void User::unsetAdminChannel(int fd, std::string OwnerChannel)
{
    for(int i = 0; i < (int)_user[fd].OwnerChannel.size(); i++)
    {
        if(_user[fd].OwnerChannel[i].data() == OwnerChannel)
            _user[fd].OwnerChannel.erase(_user[fd].OwnerChannel.begin() + i);
    }
}

void User::setAdminChannel(int fd, std::string OwnerChannel)
{
    for(int i = 0; i < (int)_user[fd].OwnerChannel.size(); i++)
    {
        if(_user[fd].OwnerChannel[i].data() == OwnerChannel)
            return ;
    }
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

void User::setBoolReverse(std::string channel, bool channelStruct::*flag, bool val)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel)
            _userChannel[i].*flag = val;
    }
}

void User::CreateChannel(std::string channel)
{
    channelStruct newChannel = {channel, "", true, "", false, false, 0, -1};
    _userChannel.push_back(newChannel);
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

void User::setLimitChannel(std::string channel, int nb)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel)
            _userChannel[i].maxUser = nb;
    }
    std::cout << "New limit -> " << nb;
}

bool User::getIfChannelNotFull(std::string channel)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel)
        {
            if(_userChannel[i].actualUser > _userChannel[i].maxUser && _userChannel[i].maxUser != -1)
                return(false);
            else
            {
                _userChannel[i].actualUser++;
                return(true);
            }
        }
    }
    return(false);
}

bool User::getIfRequirePassword(std::string channel)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel)
            return(_userChannel[i].passwordActive);
    }
    return(false);
}

bool User::checkIfPasswordValid(std::string password, std::string channel)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel && _userChannel[i].password.data() == password)
            return(true);
    }
    return(false);
}

void User::setPassword(std::string channel, std::string password)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName.data() == channel)
            _userChannel[i].password = password;
    }
}

bool User::getIfhasHabilitation(int id)
{
    if(_user[id].hasProvidePassword && _user[id].hasProvideNickName &&_user[id].hasProvideUserName)
        return(true); 
    return(false);
}

void User::getPasswordValidity(int id, std::string password, Server *srv)
{ 
    if(srv->getServerPassword().data() == password)
        _user[id].hasProvidePassword = true;
}

bool User::getIfUserIsAuthenticate(int id)
{
    return(_user[id].isAuthenticate);
}

int User::getCredit(int id)
{
    return(_user[id].credit);
}

void User::removeACredit(int id)
{
    _user[id].credit--;    
}

void User::getNameValidity(int id, std::string UserName, std::string userData::*NameType, bool userData::*NameBool)
{
    std::string originalName = UserName;
    int suffix = 0;

    while (true)
    {
        bool nameTaken = false;
        for (size_t i = 0; i < _user.size(); ++i)
        {
            if (i != (size_t)id && _user[i].*NameType == UserName)
            {
                nameTaken = true;
                break;
            }
        }
        if (!nameTaken)
            break;

        ++suffix;
        std::ostringstream oss;
        oss << suffix;
        UserName = originalName + "_" + oss.str();
    }
    _user[id].*NameType = UserName;
    _user[id].*NameBool = true;
}
