#ifndef MAIN_HEADER_HPP
# define MAIN_HEADER_HPP

/* CPP */
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
/* C */
#include <poll.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* My_files */
#include "server.hpp"
#include "user.hpp"

/* Server */
void ServerInit(char **argv);

/* Main */
void error(std::string ErrorMsg);

#endif