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

void User::getAllDataUser()
{
    /* Faut la recoder je penses */
    std::cout << "--------------\n";
    for(int i = 0; i < (int)_user.size(); i++)
    {
        if(_user[i].name != "")
            std::cout << _user[i].name << "\n";
    }
    std::cout << "--------------\n";
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
