#include "ContentOpsInfra/MySQLCplRepo.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "Data/Cpl.h"

std::string MySQLCplRepo::_database = "ice";
std::string MySQLCplRepo::_table = "cpl";

std::unique_ptr<ResultQuery> MySQLCplRepo::getCpls()
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    // Utiliser le système de requêtes standard pour une requête simple sans jointure
    Query* query = Cpl::getQuery();
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query));
    delete query;
    
    return result;
}

std::unique_ptr<ResultQuery> MySQLCplRepo::getCpl(int id)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    Query* query = Cpl::getQuery(&id);
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query));
    delete query;
    
    return result;
}

std::unique_ptr<ResultQuery> MySQLCplRepo::getCplByUuid(const std::string& uuid)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();

    // On utilise le constructeur standard de Query pour une requête simple avec condition
    Query* query = new Query(Query::SELECT, _database, _table);

    // On ajoute les champs à sélectionner
    query->addParameter("id", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    query->addParameter("uuid", nullptr, "string");
    query->addParameter("type_cpl", nullptr, "int");
    query->addParameter("path_cpl", nullptr, "string");
    query->addParameter("path_sync", nullptr, "string");
    query->addParameter("sha1_sync", nullptr, "string");

    // On ajoute la condition sur l'UUID
    query->addWhere("uuid", uuid, "string");

    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query));
    delete query;

    return result;
}

std::unique_ptr<ResultQuery> MySQLCplRepo::getCplsByScript(int scriptId)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    query->addParameter("id", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    query->addParameter("uuid", nullptr, "string");
    query->addParameter("type_cpl", nullptr, "int");
    query->addParameter("path_cpl", nullptr, "string");
    query->addParameter("path_sync", nullptr, "string");
    query->addParameter("sha1_sync", nullptr, "string");
    
    std::string customSQL = "SELECT c.id, c.name, c.uuid, c.type_cpl, c.path_cpl, c.path_sync, c.sha1_sync "
                           "FROM " + _database + "." + _table + " c "
                           "JOIN " + _database + ".link_cpl_script lcs ON c.id = lcs.id_cpl "
                           "WHERE lcs.id_script = " + std::to_string(scriptId);
    query->setCustomSQL(customSQL);
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLCplRepo::getCplsByRelease(int releaseId)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    query->addParameter("id", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    query->addParameter("uuid", nullptr, "string");
    query->addParameter("type_cpl", nullptr, "int");
    query->addParameter("path_cpl", nullptr, "string");
    query->addParameter("path_sync", nullptr, "string");
    query->addParameter("sha1_sync", nullptr, "string");
    std::string customSQL = "SELECT c.id, c.name, c.uuid, c.type_cpl, c.path_cpl, c.path_sync, c.sha1_sync "
                           "FROM " + _database + "." + _table + " c "
                           "JOIN " + _database + ".link_cpl_release lcr ON c.id = lcr.id_cpl "
                           "WHERE lcr.id_release = " + std::to_string(releaseId);
    query->setCustomSQL(customSQL);
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLCplRepo::getCplsByRelease(int contentId, int typeId, int localisationId)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    query->addParameter("id", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    query->addParameter("uuid", nullptr, "string");
    query->addParameter("type_cpl", nullptr, "int");
    query->addParameter("path_cpl", nullptr, "string");
    query->addParameter("path_sync", nullptr, "string");
    query->addParameter("sha1_sync", nullptr, "string");
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("id_localisation", nullptr, "int");
    std::string customSQL = "SELECT c.id, c.name, c.uuid, c.type_cpl, c.path_cpl, c.path_sync, c.sha1_sync, c.id_content, c.id_type, c.id_localisation "
                           "FROM " + _database + "." + _table + " c "
                           "WHERE c.id_content = " + std::to_string(contentId) +
                           " AND c.id_type = " + std::to_string(typeId) +
                           " AND c.id_localisation = " + std::to_string(localisationId);
    query->setCustomSQL(customSQL);
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLCplRepo::getUnlinkedCpls()
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    query->addParameter("id", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    query->addParameter("uuid", nullptr, "string");
    query->addParameter("type_cpl", nullptr, "int");
    query->addParameter("path_cpl", nullptr, "string");
    query->addParameter("path_sync", nullptr, "string");
    query->addParameter("sha1_sync", nullptr, "string");
    
    std::string customSQL = "SELECT c.id, c.name, c.uuid, c.type_cpl, c.path_cpl, c.path_sync, c.sha1_sync "
                           "FROM " + _database + "." + _table + " c "
                           "LEFT JOIN " + _database + ".link_cpl_script lcs ON c.id = lcs.id_cpl "
                           "LEFT JOIN " + _database + ".link_cpl_release lcr ON c.id = lcr.id_cpl "
                           "WHERE lcs.id_cpl IS NULL AND lcr.id_cpl IS NULL";
    query->setCustomSQL(customSQL);
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLCplRepo::getCplsBySite(int siteId)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    query->addParameter("id", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    query->addParameter("uuid", nullptr, "string");
    query->addParameter("type_cpl", nullptr, "int");
    query->addParameter("path_cpl", nullptr, "string");
    query->addParameter("path_sync", nullptr, "string");
    query->addParameter("sha1_sync", nullptr, "string");
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("id_localisation", nullptr, "int");
    
    std::string customSQL = "SELECT c.id, c.name, c.uuid, c.type_cpl, c.path_cpl, c.path_sync, c.sha1_sync, c.id_content, c.id_type, c.id_localisation "
                           "FROM " + _database + "." + _table + " c "
                           "JOIN " + _database + ".server_pair_configuration spc ON c.id_serv_pair_config = spc.id_serv_pair_config "
                           "WHERE spc.id_site = " + std::to_string(siteId);
    query->setCustomSQL(customSQL);
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
} 