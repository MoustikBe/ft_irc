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
    
    /* user/channel.cpp */
    void        CreateChannel(std::string channel);
    bool        checkIfPasswordValid(std::string password, std::string channel);
    void        getNameValidity(int id, std::string option, std::string userData::*NameType, bool userData::*NameBool);

    /* user/channelGetter.cpp */
    bool        getIfhasHabilitation(int id);
    int         getChannelId(std::string channel);
    bool        getIfChannelExist(std::string channel);
    bool        getIfChannelNotFull(std::string channel);
    bool        getIfRequirePassword(std::string channel);
    bool        getIfIsOnlyInvitation(std::string channel);
    void        getPasswordValidity(int id, std::string password, Server *srv);

    /* user/channelSetter.cpp */
    void        setLimitChannel(std::string channel, int nb);
    void        setTopicChannel(std::string Topic, int currentChannel);
    void        setPassword(std::string channel, std::string password);
    void        setBoolReverse(std::string channel, bool channelStruct::*flag, bool val, std::string message);

    /* user/user.cpp */
    void        SendNotification(int id, std::string message);
    void        SendMessageForAllUser(std::string channel, std::string message, int id);
    int         getUserChannelId(const std::vector<std::string>& channels, const std::string& channelName);

    /* user/userAdmin.cpp */
    bool        getPrivilege(std::string channel, int id);
    void        setAdminChannel(int fd, std::string OwnerChannel);
    void        unsetAdminChannel(int fd, std::string OwnerChannel);

    /* user/userGetter.cpp */
    int         getLen();
    void        getAllDataUser();
    int         getCredit(int id);
    int         getUserFd(int id);
    std::string getUserName(int fd);
    bool        getChannelTopicStatus(int index);
    int         getUserIdByName(std::string name);

    /* user/userInChannel.cpp */
    void        setChanel(std::string ChanelName, int fd);
    void        removeChannel(std::string channel, int id);
    bool        getIfUserIsInChannel(std::string channelName, int id);

    /* user/userInvitedChannel.cpp */
    bool        getIfChannelInvitation(std::string channel, int id);
    void        setInvitationChannel(std::string Invitation, int id);

    /* user/userSetter.cpp */
    void        setSocket(int fd);
    void        removeACredit(int id);
    void        setUsername(std::string name, int id);

};

#endif