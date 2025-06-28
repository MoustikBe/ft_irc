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
    for(int i = 0; i < _user.size(); i++)
        count++;
    return(count);
}

int User::getUserFd(int id)
{
    return(_user[id].socketUser);
}