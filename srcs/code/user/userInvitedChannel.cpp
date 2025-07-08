#include "../../header/mainHeader.hpp"

void User::setInvitationChannel(std::string Invitation, int id)
{
    int i = getUserChannelId(_user[id].invitedChannel, Invitation);
    if(i != -1)
        return ;
    _user[id].invitedChannel.push_back(Invitation);
}

bool User::getIfChannelInvitation(std::string channel, int id)
{
    int i = getUserChannelId(_user[id].invitedChannel, channel);
    if(i != -1)
        return true;
    return false;
}
