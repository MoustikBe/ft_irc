#ifndef USER_HPP
# define USER_HPP

#include <iostream>

struct userData
{
    std::string name;
    std::string chanel;
    int         socketUser;
    bool        online;
};

class User
{
private:
    std::map<int, userData> _user;

public:
    User(){};
    ~User(){};
    void        setUsername(std::string name, int id);
    void        setSocket(int fd);
    void        setChanel(std::string ChanelName, int fd);
    int         getLen();
    void        getAllDataUser();
    int         getUserFd(int id);
    std::string getChanelName(int id);
    std::string getUserName(int fd);
};

#endif