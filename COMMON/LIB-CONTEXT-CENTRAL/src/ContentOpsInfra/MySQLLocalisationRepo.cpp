#include "ContentOpsInfra/MySQLLocalisationRepo.h"
#include "ContentOpsInfra/MySQLDBConnection.h"

std::string MySQLLocalisationRepo::_database = "ice";
std::string MySQLLocalisationRepo::_table = "localisation";

std::unique_ptr<ResultQuery> MySQLLocalisationRepo::getLocalisations()
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    // Ajouter les paramètres au schéma pour que DatabaseConnector puisse mapper les colonnes
    query->addParameter("id_localisation", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    
    std::string customSQL = "SELECT id_localisation, name FROM " + _database + "." + _table + " ORDER BY id_localisation";
    query->setCustomSQL(customSQL);
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLLocalisationRepo::getLocalisation(int id)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    // Ajouter les paramètres au schéma pour que DatabaseConnector puisse mapper les colonnes
    query->addParameter("id_localisation", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    
    std::string customSQL = "SELECT id_localisation, name FROM " + _database + "." + _table + " WHERE id_localisation = ?";
    query->setCustomSQL(customSQL);
    query->addParameter("id", &id, "int");
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
} 