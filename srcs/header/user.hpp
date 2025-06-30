#ifndef USER_HPP
# define USER_HPP

#include <iostream>

struct channelStruct
{
    std::string channelName;
    std::string Topic;
    bool        TopicAdmin;
    std::string password;
    bool        passwordActive;
    size_t      maxUser;
};

struct userData
{
    int         socketUser;
    bool        online;
    std::string name;
    std::vector<channelStruct> channel;
    std::vector<std::string> OwnerChannel;
};

class User
{
private:
    std::map<int, userData> _user;

public:
    User(){};
    ~User(){};
    void        removeChannel(std::string channel, int id);
    void        setUsername(std::string name, int id);
    void        setSocket(int fd);
    void        setAdminChannel(int fd, std::string OwnerChannel);
    void        setChanel(std::string ChanelName, int fd);
    void        setTopicChannel(std::string Topic, int id, int currentChannel);
    int         getLen();
    void        getAllDataUser();
    int         getUserFd(int id);
    bool        getIfChannelExist(std::string channelName, int id);
    bool        getPrivilege(std::string channel, int id);
    int         getChannelIndex(std::string channel, int id);
    bool        getChannelTopicStatus(int id, int index);
    std::string getUserName(int fd);
};

#endif