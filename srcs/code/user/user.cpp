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
    for(int i = 0; i < (int)_user.size(); i++)
    {
        if(_user[i].name != "")
            std::cout << _user[i].name << "\n";
    }
}
