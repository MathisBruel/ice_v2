#pragma once
#include "DatabaseConnector.h"
#include "Query.h"
#include "ResultQuery.h"
#include "Poco/Logger.h"
class MySQLDBConnection
{    
private:
    DatabaseConnector* _database;
    MySQLDBConnection();
    ~MySQLDBConnection();
public:
    bool InitConnection();
    ResultQuery* ExecuteQuery(Query* query);
};