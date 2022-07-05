#include "base64Encoder.h"

Base64Encoder::Base64Encoder() 
{
    stop = false;
}
Base64Encoder::~Base64Encoder() {}

void Base64Encoder::initialize(Application& self) {
    Application::initialize(self);
}
void Base64Encoder::reinitialize(Application& self) {
    Application::reinitialize(self);
}
void Base64Encoder::uninitialize()
{
    Poco::Util::Application::uninitialize();
}

int Base64Encoder::main(const std::vector<std::string> &arguments)
{

    while (!stop) {

        // -- print for users
        std::cout << "Enter new data to encrypt : " << std::endl;
        std::cout << "Encrypt username + password : user=<username>:password=<password>" << std::endl;
        std::cout << "Exit tool : \"exit\"" << std::endl << std::endl;

        // -- get string and parse
        std::string str;
        std::getline(std::cin, str);
        parseCmdLine(str);
        Timer::crossUsleep(500000);

        std::cout << std::endl;
    }

    return EXIT_OK;
}

void Base64Encoder::parseCmdLine(std::string cmd)
{
    // -- handle exit
    if (cmd == "exit") {
        stop = true;
        return;
    }

    std::string toEncode;

    int posSep = cmd.find(":", 0);
    if (posSep == std::string::npos) {
        toEncode = cmd;
    }
    else {
        std::string paramUser = cmd.substr(0, posSep);
        std::string paramPass = cmd.substr(posSep+1);

        // -- parsing user
        int posUser = paramUser.find("=", 0);
        if (posUser == std::string::npos) {
            std::cout << "Invalid command : param user !" << std::endl;
            return;
        }
        std::string user = paramUser.substr(posUser+1);

        // -- parsing password
        int posPass = paramPass.find("=", 0);
        if (posPass == std::string::npos) {
            std::cout << "Invalid command : param pass !" << std::endl;
            return;
        }
        std::string pass = paramPass.substr(posPass+1);

        toEncode = user;
        toEncode += "\n";
        toEncode += pass;
    }

    std::cout << "ENCODE : " << std::endl;
    std::string firstEncode = Converter::encodeBase64(toEncode);
    std::cout << "First encode : " << firstEncode << std::endl;
    std::string secondEncode = Converter::encodeBase64(firstEncode);
    std::cout << "Second encode : " << secondEncode << std::endl << std::endl;
}