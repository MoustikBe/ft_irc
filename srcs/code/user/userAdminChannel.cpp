#include "../../header/mainHeader.hpp"

bool User::getPrivilege(std::string channel, int id)
{
    int i = getUserChannelId(_user[id].OwnerChannel, channel);
    if(i != -1)
        return true;
    return false;
}

void User::unsetAdminChannel(int fd, std::string OwnerChannel)
{
    int i = getUserChannelId(_user[fd].OwnerChannel, OwnerChannel);

    if(i != -1)
        _user[fd].OwnerChannel.erase(_user[fd].OwnerChannel.begin() + i);
}

void User::setAdminChannel(int fd, std::string OwnerChannel)
{
    int i = getUserChannelId(_user[fd].OwnerChannel, OwnerChannel);

    if(i != -1)
        return ;
    _user[fd].OwnerChannel.push_back(OwnerChannel);
}
