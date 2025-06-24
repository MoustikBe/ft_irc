#include "../header/mainHeader.hpp"


void error(std::string ErrorMsg)
{
    std::cout << ErrorMsg << "\n";
    exit(1);
}

bool VerifArguments(char **argv)
{
    int port = atoi(argv[1]);

    if(port < 1024 || port > 65535)
        return(std::cout << "\033[0;31mPort invalid\n", false);
    return(true);
}

int main(int argc, char** argv)
{   
    if(argc != 3)
        return(std::cout << "\033[0;31m Invalid number of arguments\n", 1);
    else if(!VerifArguments(argv))
        return(1);
    Server srv(atoi(argv[1]));
    sockaddr_in address;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_port = htons(srv.getPort());
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(serverSocket, (struct sockaddr*)&address, sizeof(address));
    listen(serverSocket, 10);                                                                                                                                                                                                                                                                                                                                                                           
    int clientSocket = accept(serverSocket, NULL, NULL);
    std::string welcome = "[CONNECTED to ft_IRCS] | Welcome !\n";
    send(clientSocket, welcome.c_str(), welcome.size(), 0);
    char *buffer; 
    buffer = new char[1000];
    while(buffer)
    {
        //delete buffer;
        buffer = new char[1000];
        recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout <<  ">" << buffer << "\n";
        delete buffer;
    }
    close(serverSocket);
    return(0);
}