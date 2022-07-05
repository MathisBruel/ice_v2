#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#pragma once

// -- COMMANDS
#include "Poco/Net/HTTPServer.h"
#include "httpRepoReceiver.h"

#include "portable_timer.h"

class RepoContext
{

public:

    static RepoContext* getCurrentContext();

    bool startHttpCmdHandler();
    HttpRepoHandler* getCommandHandler() {return commandHandler;}

private:

    RepoContext();
    ~RepoContext();

    static RepoContext* context;

    HttpRepoHandler* commandHandler;
};