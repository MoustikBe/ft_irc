#ifndef SERVER_HPP
# define SERVER_HPP



class Server
{
private:
    int _port;
public:
    Server(){};
    Server(int port) : _port(port){};
    ~Server(){};
    int getPort(void) {return(_port);};
};


#endif