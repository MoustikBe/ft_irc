#include "../../header/mainHeader.hpp"

void User::CreateChannel(std::string channel)
{
    channelStruct newChannel = {channel, "", true, "", false, false, 0, -1};
    _userChannel.push_back(newChannel);
}

bool User::checkIfPasswordValid(std::string password, std::string channel)
{
    int i = getChannelId(channel);
    if(i == -1)
        return(false);
    if(_userChannel[i].password.data() == password)
        return(true);
    return(false);
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
    if(getIfhasHabilitation(id))
        SendNotification(id, "WARNING New USER or NICK name is -> " + _user[id].*NameType + "\r\n");
}
