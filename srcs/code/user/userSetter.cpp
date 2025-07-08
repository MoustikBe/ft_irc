#include "../../header/mainHeader.hpp"

void User::setSocket(int fd)
{
    _user[fd].credit = 4;
    _user[fd].hasProvidePassword = false;
    _user[fd].hasProvideNickName = false;
    _user[fd].hasProvideUserName = false;
    _user[fd].socketUser = fd;
}

void User::setUsername(std::string name, int fd)
{
    std::cout << "Username " << name << " Set on the user[fd] " << fd << "\n"; 
    _user[fd].name = name;
}

void User::removeACredit(int id)
{
    _user[id].credit--;    
}
