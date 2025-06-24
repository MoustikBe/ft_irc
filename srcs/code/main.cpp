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
    User usr1;
    sockaddr_in address;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_port = htons(srv.getPort());
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(serverSocket, (struct sockaddr*)&address, sizeof(address));
    listen(serverSocket, 10);                                                                                                                                                                                                                                                                                                                                                                           
    int clientSocket = accept(serverSocket, NULL, NULL);
    std::string welcome = "001\r\n";
    std::string timeInfo = "TIME : \r\n";
    send(clientSocket, welcome.c_str(), welcome.size(), 0);
    
    /* Data echange beetwen client <-> server */
    char *buffer;
    while(strncmp(buffer, "QUIT", 4))
    {
        buffer = new char[1000];
        recv(clientSocket, buffer, sizeof(buffer), 0);
        if(strncmp(buffer, "time", 4) == 0)
            send(clientSocket, timeInfo.c_str(), timeInfo.size(), 0); 
        if(strncmp(buffer, "NICK", 4) == 0)
            usr1.setUserName(buffer); /* Du coup c'est comme ca qu'on gere les user*/
        for(int i = 0; buffer[i]; i++)
            std::cout << buffer[i] << "";
    }
    /* Data echange beetwen client <-> server */
    std::cout << usr1.getUserName() << "\n";
    close(serverSocket);
    return(0);
}