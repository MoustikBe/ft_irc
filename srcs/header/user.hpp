#ifndef USER_HPP
# define USER_HPP

#include <iostream>

class User
{
private:
    std::string _UserName;

public:
    User(){};
    User(std::string UserName) : _UserName(UserName){};
    ~User(){};
    inline std::string getUserName() {return(_UserName);};
    inline void setUserName(std::string UserName) {_UserName = UserName;};
};

#endif