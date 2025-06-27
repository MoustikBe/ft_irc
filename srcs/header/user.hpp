#ifndef USER_HPP
# define USER_HPP

#include <iostream>

class User
{
private:
    std::vector<std::string> _user;

public:
    User(){};
    ~User(){};
    inline void setUsername(std::string username, int it) {_user[it] = username;};
};

#endif