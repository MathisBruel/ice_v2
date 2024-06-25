#include "MySQLDBConnection.h"

MysqlDBConnection::MysqlDBConnection()
{
    database = nullptr;
}

MysqlDBConnection::~MysqlDBConnection()
{
    if (database != nullptr)
    {
        delete database;
    }
}

bool MysqlDBConnection::initConnection()
{
    database = new DatabaseConnector();

    if (!database->isSessionActive()) {
        Poco::Logger::get("CentralContext").error("Connection to database mySQL in error", __FILE__, __LINE__);
        return false;
    }

    return true;
}