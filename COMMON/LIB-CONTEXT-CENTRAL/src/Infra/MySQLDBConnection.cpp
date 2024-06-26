#include "Infra/MySQLDBConnection.h"

MySQLDBConnection::MySQLDBConnection()
{
    database = nullptr;
}

MySQLDBConnection::~MySQLDBConnection()
{
    if (database != nullptr)
    {
        delete database;
    }
}

bool MySQLDBConnection::initConnection()
{
    database = new DatabaseConnector();

    if (!database->isSessionActive()) {
        Poco::Logger::get("CentralContext").error("Connection to database mySQL in error", __FILE__, __LINE__);
        return false;
    }

    return true;
}

ResultQuery* MySQLDBConnection::executeQuery(Query* query) {
    
}