#include "Poco/Util/Application.h"
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/OptionCallback.h>
#include "Converter.h"
#include "portable_timer.h"

#include <iostream>
#if defined(__linux__)
    #include "unistd.h"
#endif

class Base64Encoder : public Poco::Util::Application
{

public:

    Base64Encoder();
    ~Base64Encoder();

protected:

    void initialize(Application& self);
    void reinitialize(Application& self);
    void uninitialize();

    virtual int main(const std::vector<std::string> &args);

private:
    bool stop;

    void parseCmdLine(std::string cmd);
};

POCO_APP_MAIN(Base64Encoder);