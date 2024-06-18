#pragma once
#include "DatabaseConnector.h"
#include "Query.h"
#include "ResultQuery.h"

class MySQLDBConnection
{    
private:
    DatabaseConnector* database;
    MySQLDBConnection();
    ~MySQLDBConnection();
public:
    bool initConnection();
    ResultQuery* executeQuery(Query* query);
};