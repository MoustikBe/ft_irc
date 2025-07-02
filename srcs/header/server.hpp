#ifndef SERVER_HPP
# define SERVER_HPP


class Server
{
private:
    int         _port;
    std::string _password; 
public:
    Server(){};
    Server(int port, std::string password) : _port(port), _password(password){};
    ~Server(){};
    int         getPort(void) {return(_port);};
    std::string getServerPassword(void) {return(_password);};
};


#endif