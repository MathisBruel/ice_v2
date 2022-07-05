#include "base64Decoder.h"

Base64Converter::Base64Converter() 
{
    stop = false;
}
Base64Converter::~Base64Converter() {}

void Base64Converter::initialize(Application& self) {
    Application::initialize(self);
}
void Base64Converter::reinitialize(Application& self) {
    Application::reinitialize(self);
}
void Base64Converter::uninitialize()
{
    Poco::Util::Application::uninitialize();
}

int Base64Converter::main(const std::vector<std::string> &arguments)
{

    while (!stop) {

        // -- print for users
        std::cout << "Enter new encrypted data : " << std::endl;
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

void Base64Converter::parseCmdLine(std::string cmd)
{
    // -- handle exit
    if (cmd == "exit") {
        stop = true;
        return;
    }

    std::cout << "DECODE : " << std::endl;
    std::string firstDecode = Converter::decodeBase64(cmd);
    std::cout << "First decode : " << firstDecode << std::endl;
    std::string secondDecode = Converter::decodeBase64(firstDecode);
    std::cout << "Second decode : " << secondDecode << std::endl << std::endl;
}