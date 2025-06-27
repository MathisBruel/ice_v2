#pragma once
#include "DatabaseConnector.h"
#include "Query.h"
#include "ResultQuery.h"
#include "Poco/Logger.h"
class MySQLDBConnection
{    
private:
    DatabaseConnector* _database;
public:
    MySQLDBConnection();
    MySQLDBConnection(DatabaseConnector* database);
    ~MySQLDBConnection();
    bool InitConnection();
    ResultQuery* ExecuteQuery(Query* query);
};