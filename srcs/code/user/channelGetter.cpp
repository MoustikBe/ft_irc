#include "../../header/mainHeader.hpp"

int User::getChannelId(std::string channel)
{
    for(int i = 0; i < (int)_userChannel.size(); i++)
    {
        if(_userChannel[i].channelName == channel)
            return(i);
    }
    return(-1);
}

bool User::getIfChannelExist(std::string channel)
{
    if(getChannelId(channel) != -1)
        return(true);
    return(false);
}

void User::getPasswordValidity(int id, std::string password, Server *srv)
{ 
    if(srv->getServerPassword().data() == password)
        _user[id].hasProvidePassword = true;
}

bool User::getIfIsOnlyInvitation(std::string channel)
{
    int i = getChannelId(channel);

    if(i == -1)
        return(false);
    else if(_userChannel[i].channelName.data() == channel && _userChannel[i].InviteOnly)
        return(true);
    return(false);
}

bool User::getIfChannelNotFull(std::string channel)
{
    int i = getChannelId(channel);
    if(i == -1)
        return(false);
    if(_userChannel[i].actualUser >= _userChannel[i].maxUser && _userChannel[i].maxUser != -1)
        return(false);
    else
        return(_userChannel[i].actualUser++, true);
    return(false);
}

bool User::getIfRequirePassword(std::string channel)
{
    int i = getChannelId(channel);
    if(i == -1)
        return(false);
    return(_userChannel[i].passwordActive);
}

bool User::getIfhasHabilitation(int id)
{
    if(_user[id].hasProvidePassword && _user[id].hasProvideNickName &&_user[id].hasProvideUserName)
    {
        if(_user[id].accessRequest == 0)
        {
            /* Je pourrais faire une fonction pour afficher tout de maniere propre pour le bot */
            SendNotification(id, "INFO, Connection to the server succeeded.\r\n");
            SendNotification(id, "INFO, Your default User Name -> " + _user[id].username + "\r\n");
            SendNotification(id, "INFO, Your default Nick Name -> " + _user[id].name + "\r\n");
            SendNotification(id, "INFO, If you want to change them use /USER [new_name] /NICK [new_name] \r\n");
            _user[id].accessRequest++;
        }
        return(true); 
    }
    return(false);
}
