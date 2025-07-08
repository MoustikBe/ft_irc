#include "../../header/mainHeader.hpp"

void User::setTopicChannel(std::string Topic, int currentChannel)
{
    _userChannel[currentChannel].Topic = Topic;
    std::cout << "New Topic of channel -> " << _userChannel[currentChannel].Topic << "\n";
}

void User::setBoolReverse(std::string channel, bool channelStruct::*flag, bool val, std::string message)
{
    int i = getChannelId(channel);

    if(i == -1)
        return ;
    if(_userChannel[i].*flag == val)
        return ;
    _userChannel[i].*flag = val;
    SendMessageForAllUser(channel, message, -1);
}

void User::setLimitChannel(std::string channel, int nb)
{
    int i = getChannelId(channel);
    if(i == -1)
        return ;
    _userChannel[i].maxUser = nb;
    std::cout << "New limit -> " << nb;
}

void User::setPassword(std::string channel, std::string password)
{
    int i = getChannelId(channel);
    if(i != -1)
        _userChannel[i].password = password;
}
