#ifndef SERVER_HPP
# define SERVER_HPP


class Server
{
private:
    int         _port;
    std::string _password; 
public:
    Server(){};
    Server(const Server &other) : _port(other._port), _password(other._password){};
    inline Server& operator=(const Server &other) {if(this != &other){_port=other._port;_password=other._password;}return(*this);};
    Server(int port, std::string password) : _port(port), _password(password){};
    ~Server(){};
    int         getPort(void) {return(_port);};
    std::string getServerPassword(void) {return(_password);};
};


#endif