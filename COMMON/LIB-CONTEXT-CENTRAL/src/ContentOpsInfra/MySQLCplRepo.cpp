#include "ContentOpsInfra/MySQLCplRepo.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "Data/Cpl.h"

std::string MySQLCplRepo::_database = "ice";
std::string MySQLCplRepo::_table = "cpl";

MySQLCplRepo::MySQLCplRepo() {
    _query = nullptr;
}

MySQLCplRepo::~MySQLCplRepo() {}

std::unique_ptr<ResultQuery> MySQLCplRepo::getCpls()
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
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

    Query* query = new Query(Query::SELECT, _database, _table);

    query->addParameter("id", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    query->addParameter("uuid", nullptr, "string");
    query->addParameter("type_cpl", nullptr, "int");
    query->addParameter("path_cpl", nullptr, "string");
    query->addParameter("path_sync", nullptr, "string");
    query->addParameter("sha1_sync", nullptr, "string");

    query->addWhereParameter("uuid", (void*)&uuid, "string");

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

std::unique_ptr<Query> MySQLCplRepo::MySQLcreate(COB_Cpl* cpl) {
    if (!cpl) return nullptr;
    std::unique_ptr<Query> createQuery = std::make_unique<Query>(Query::INSERT, _database, _table);
    int id = cpl->GetId();
    std::string name = cpl->GetName();
    std::string uuid = cpl->GetUuid();
    int type_cpl = cpl->GetTypeCpl();
    std::string path_cpl = cpl->GetPathCpl();
    std::string path_sync = cpl->GetPathSync();
    std::string sha1_sync = cpl->GetSha1Sync();
    long long id_serv_pair_config = cpl->GetIdServPairConfig();
    long long id_content = cpl->GetIdContent();
    long long id_type = cpl->GetIdType();
    long long id_localisation = cpl->GetIdLocalisation();

    createQuery->addParameter("name", &name, "string");
    createQuery->addParameter("uuid", &uuid, "string");
    createQuery->addParameter("type_cpl", &type_cpl, "int");
    createQuery->addParameter("path_cpl", &path_cpl, "string");
    createQuery->addParameter("path_sync", &path_sync, "string");
    createQuery->addParameter("sha1_sync", &sha1_sync, "string");
    createQuery->addParameter("id_serv_pair_config", &id_serv_pair_config, "int");
    createQuery->addParameter("id_content", &id_content, "int");
    createQuery->addParameter("id_type", &id_type, "int");
    createQuery->addParameter("id_localisation", &id_localisation, "int");
    return createQuery;
}

std::unique_ptr<Query> MySQLCplRepo::MySQLupdate(COB_Cpl* cpl) {
    if (!cpl) return nullptr;
    int id = cpl->GetId();
    if (id == -1) return nullptr;
    std::unique_ptr<Query> updateQuery = std::make_unique<Query>(Query::UPDATE, _database, _table);
    std::string name = cpl->GetName();
    std::string uuid = cpl->GetUuid();
    int type_cpl = cpl->GetTypeCpl();
    std::string path_cpl = cpl->GetPathCpl();
    std::string path_sync = cpl->GetPathSync();
    std::string sha1_sync = cpl->GetSha1Sync();
    long long id_serv_pair_config = cpl->GetIdServPairConfig();
    long long id_content = cpl->GetIdContent();
    long long id_type = cpl->GetIdType();
    long long id_localisation = cpl->GetIdLocalisation();

    updateQuery->addParameter("name", &name, "string");
    updateQuery->addParameter("uuid", &uuid, "string");
    updateQuery->addParameter("type_cpl", &type_cpl, "int");
    updateQuery->addParameter("path_cpl", &path_cpl, "string");
    updateQuery->addParameter("path_sync", &path_sync, "string");
    updateQuery->addParameter("sha1_sync", &sha1_sync, "string");
    updateQuery->addParameter("id_serv_pair_config", &id_serv_pair_config, "int");
    updateQuery->addParameter("id_content", &id_content, "int");
    updateQuery->addParameter("id_type", &id_type, "int");
    updateQuery->addParameter("id_localisation", &id_localisation, "int");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}

std::unique_ptr<Query> MySQLCplRepo::MySQLremove(COB_Cpl* cpl) {
    if (!cpl) return nullptr;
    int id = cpl->GetId();
    if (id == -1) return nullptr;
    std::unique_ptr<Query> removeQuery = std::make_unique<Query>(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id", &id, "int");
    return removeQuery;
}

void MySQLCplRepo::Create(COB_Cpl* cpl) {
    _query = MySQLcreate(cpl);
    if (_query) {
        std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
        dbConn->InitConnection();
        ResultQuery* result = dbConn->ExecuteQuery(_query.get());
        if (result && result->isValid()) {
            int newId = result->getLastInsertedId();
            if (newId != -1) {
                cpl->SetId(newId);
            }
        }
    }
}

void MySQLCplRepo::Remove(COB_Cpl* cpl) {
    _query = MySQLremove(cpl);
    if (_query) {
        std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
        dbConn->InitConnection();
        dbConn->ExecuteQuery(_query.get());
    }
}

void MySQLCplRepo::Update(COB_Cpl* cpl) {
    _query = MySQLupdate(cpl);
    if (_query) {
        std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
        dbConn->InitConnection();
        dbConn->ExecuteQuery(_query.get());
    }
} 