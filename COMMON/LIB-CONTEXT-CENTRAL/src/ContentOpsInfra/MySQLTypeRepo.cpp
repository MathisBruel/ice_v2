#include "ContentOpsInfra/MySQLTypeRepo.h"
#include "ContentOpsInfra/MySQLDBConnection.h"

std::string MySQLTypeRepo::_database = "ice";
std::string MySQLTypeRepo::_table = "type";

std::unique_ptr<ResultQuery> MySQLTypeRepo::getTypes()
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    // Ajouter les paramètres au schéma pour que DatabaseConnector puisse mapper les colonnes
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    
    std::string customSQL = "SELECT id_type, name FROM " + _database + "." + _table + " ORDER BY id_type";
    query->setCustomSQL(customSQL);
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLTypeRepo::getType(int id)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    // Ajouter les paramètres au schéma pour que DatabaseConnector puisse mapper les colonnes
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    
    std::string customSQL = "SELECT id_type, name FROM " + _database + "." + _table + " WHERE id_type = ?";
    query->setCustomSQL(customSQL);
    query->addParameter("id", &id, "int");
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
} 