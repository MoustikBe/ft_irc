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

/* Server */
void ServerInit(char **argv);

/* Main */
void error(std::string ErrorMsg);

#endif