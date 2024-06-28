#include "Infra/MySQLDBConnection.h"

MySQLDBConnection::MySQLDBConnection()
{
    _database = nullptr;
}

MySQLDBConnection::~MySQLDBConnection()
{
    if (_database != nullptr)
    {
        delete _database;
    }
}

bool MySQLDBConnection::InitConnection()
{
    _database = new DatabaseConnector();

    if (!_database->isSessionActive()) {
        Poco::Logger::get("CentralContext").error("Connection to database mySQL in error", __FILE__, __LINE__);
        return false;
    }

    return true;
}