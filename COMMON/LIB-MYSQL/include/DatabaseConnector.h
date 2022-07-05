#include <stdlib.h>
#include <iostream>

#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/MySQL/MySQLException.h"
#include "Poco/Data/DataException.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/Row.h"
#include "Poco/Logger.h"

#include "Query.h"
#include "ResultQuery.h"

#pragma once

class DatabaseConnector
{

public: 

    DatabaseConnector();
    ~DatabaseConnector();

    ResultQuery* executeQuery(Query* query);

    bool isSessionActive() {return session->isConnected() && session->isGood();}

private:

    Poco::Data::Session* session;

};