#ifndef MAIN_HEADER_HPP
# define MAIN_HEADER_HPP

/* CPP */
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
/* C */
#include <poll.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
/* My_files */
#include "server.hpp"
#include "user.hpp"

/* Server Main */
void    ServerInit(char **argv);
bool    IsChanel(std::string message);
/* Server_command.cpp*/
void    requestJoin(User *Users, std::string JoinMsg, int fd);
void    requestInvite(User *Users, std::string message, int fd);
void    requestChangeName(int fd, std::string ServerMsg, User *Users);
void    requestChanelMessage(User *Users, std::string message, int fd);
void    requestPart(User *Users, std::string ServerMsg, int fd);
void    requestMode(User *Users, std::string message, int fd);
void    requestKick(User *Users, std::string message, int fd);
void    requestTopic(User *Users, std::string message, int fd);
void    requestMessage(User *Users, std::string message, int fd);

/* Main */
void error(std::string ErrorMsg);

#endif