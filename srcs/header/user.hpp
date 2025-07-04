#ifndef USER_HPP
# define USER_HPP

#include <iostream>

struct channelStruct
{
    std::string channelName;
    std::string Topic;
    bool        TopicActive;
    std::string password;
    bool        passwordActive;
    bool        InviteOnly;
    int         actualUser;
    int         maxUser;
};

struct userData
{
    int         socketUser;
    int         credit;
    bool        online;
    bool        hasProvidePassword;
    bool        hasProvideNickName;
    bool        hasProvideUserName;
    bool        hasHabilitation;
    bool        isAuthenticate;
    int         accessRequest;
    std::string name;
    std::string username;
    std::vector<std::string> InChannel;
    std::vector<std::string> OwnerChannel;
    std::vector<std::string> invitedChannel;
};

class User
{
private:
    std::map<int, userData>     _user;
    std::vector<channelStruct>  _userChannel;

public:
    User(){};
    ~User(){};
    void        removeChannel(std::string channel, int id);
    void        setUsername(std::string name, int id);
    void        setSocket(int fd);
    void        setAdminChannel(int fd, std::string OwnerChannel);
    void        unsetAdminChannel(int fd, std::string OwnerChannel);
    void        setChanel(std::string ChanelName, int fd);
    void        setTopicChannel(std::string Topic, int currentChannel);
    void        setInvitationChannel(std::string Invitation, int id);
    void        setBoolReverse(std::string channel, bool channelStruct::*flag, bool val, std::string message);
    void        setLimitChannel(std::string channel, int nb);
    void        setPassword(std::string channel, std::string password);
    void        removeACredit(int id);
    int         getLen();
    void        getAllDataUser();
    int         getUserFd(int id);
    int         getUserIdByName(std::string name);
    bool        getIfUserIsInChannel(std::string channelName, int id);
    bool        getPrivilege(std::string channel, int id);
    int         getChannelIndex(std::string channel);
    int         getCredit(int id);
    bool        getChannelTopicStatus(int index);
    bool        getIfIsOnlyInvitation(std::string channel);
    bool        getIfChannelInvitation(std::string channel, int id);
    bool        getIfChannelExist(std::string channel);
    bool        getIfChannelNotFull(std::string channel);
    bool        getIfRequirePassword(std::string channel);
    bool        getIfhasHabilitation(int id);
    bool        getIfUserIsAuthenticate(int id);
    void        getPasswordValidity(int id, std::string password, Server *srv);
    void        getNameValidity(int id, std::string option, std::string userData::*NameType, bool userData::*NameBool);
    bool        checkIfPasswordValid(std::string password, std::string channel);
    std::string getUserName(int fd);

    void        SendNotification(int id, std::string message);

    void        SendMessageForAllUser(std::string channel, std::string message, int id);

    void        CreateChannel(std::string channel);
};

#endif