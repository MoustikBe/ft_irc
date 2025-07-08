#include "../header/mainHeader.hpp"

void error(std::string ErrorMsg)
{
    std::cout << ErrorMsg << "\n";
    exit(1);
}

bool VerifArguments(char **argv)
{
    int port                = atoi(argv[1]);
    std::string password    = argv[2];

    if(!password.data())
        return(std::cout << "\033[0;31mPassword invalid\n", false);
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
    ServerInit(argv);
    return(0);
}