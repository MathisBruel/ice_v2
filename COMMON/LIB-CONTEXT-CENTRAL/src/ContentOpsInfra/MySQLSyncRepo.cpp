#include "ContentOpsInfra/MySQLSyncRepo.h"
#include "ContentOpsInfra/MySQLDBConnection.h"

std::string MySQLSyncRepo::_database = "ice";
std::string MySQLSyncRepo::_table = "sync";

std::unique_ptr<Query> MySQLSyncRepo::MySQLcreate(COD_Sync* sync)
{
    const int* _syncId = sync->GetSyncID();
    if (_syncId[0] == -1 || _syncId[1] == -1 || _syncId[2] == -1 || _syncId[3] == -1) { return nullptr; }
    _syncPath = sync->GetSyncPath();
    auto createQuery = std::make_unique<Query>(Query::INSERT, _database, _table);
    createQuery->addParameter("id_serv_pair_config", (void*)&_syncId[0], "int");
    createQuery->addParameter("id_content", (void*)&_syncId[1], "int");
    createQuery->addParameter("id_type", (void*)&_syncId[2], "int");
    createQuery->addParameter("id_localisation", (void*)&_syncId[3], "int");
    createQuery->addParameter("path_sync", &_syncPath, "string");
    return createQuery;
}
std::unique_ptr<Query> MySQLSyncRepo::MySQLread(COD_Sync* sync)
{
    _syncId = sync->GetSyncID();
    auto readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);

    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("path_sync", nullptr, "string");
    if (*&_syncId[0] != -1 ) { readQuery->addWhereParameter("id_serv_pair_config", (void*)&_syncId[0], "int"); };
    if (*&_syncId[1] != -1 ) { readQuery->addWhereParameter("id_content", (void*)&_syncId[1], "int"); };
    if (*&_syncId[2] != -1 ) { readQuery->addWhereParameter("id_type", (void*)&_syncId[2], "int"); };
    if (*&_syncId[3] != -1 ) { readQuery->addWhereParameter("id_localisation", (void*)&_syncId[3], "int"); };

    return readQuery;
}
std::unique_ptr<Query> MySQLSyncRepo::MySQLupdate(COD_Sync* sync)
{
    _syncId = sync->GetSyncID();
    if (_syncId[0] == -1 || _syncId[1] == -1 || _syncId[2] == -1 || _syncId[3] == -1) { return nullptr; }
    _syncPath = sync->GetSyncPath();
    auto updateQuery = std::make_unique<Query>(Query::UPDATE, _database, _table);
    updateQuery->addParameter("path_sync", &_syncPath, "string");
    updateQuery->addWhereParameter("id_serv_pair_config", (void*)&_syncId[0], "int");
    updateQuery->addWhereParameter("id_content", (void*)&_syncId[1], "int");
    updateQuery->addWhereParameter("id_type", (void*)&_syncId[2], "int");
    updateQuery->addWhereParameter("id_localisation", (void*)&_syncId[3], "int");
    return updateQuery;
}
std::unique_ptr<Query> MySQLSyncRepo::MySQLremove(COD_Sync* sync)
{
    _syncId = sync->GetSyncID();
    if (_syncId[0] == -1 || _syncId[1] == -1 || _syncId[2] == -1 || _syncId[3] == -1) { return nullptr; }

    auto removeQuery = std::make_unique<Query>(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_serv_pair_config", (void*)&_syncId[0], "int");
    removeQuery->addWhereParameter("id_content", (void*)&_syncId[1], "int");
    removeQuery->addWhereParameter("id_type", (void*)&_syncId[2], "int");
    removeQuery->addWhereParameter("id_localisation", (void*)&_syncId[3], "int");
    return removeQuery;
}

std::unique_ptr<ResultQuery> MySQLSyncRepo::getSyncs() {
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::SELECT, _database, _table);
    query->addParameter("id_serv_pair_config", nullptr, "int");
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("id_localisation", nullptr, "int");
    query->addParameter("path_sync", nullptr, "string");
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLSyncRepo::getSyncsByRelease(int contentId, int typeId, int localisationId) {
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::SELECT, _database, _table);
    query->addParameter("id_serv_pair_config", nullptr, "int");
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("id_localisation", nullptr, "int");
    query->addParameter("path_sync", nullptr, "string");
    
    // Filtrer par release
    query->addWhereParameter("id_content", (void*)&contentId, "int");
    query->addWhereParameter("id_type", (void*)&typeId, "int");
    query->addWhereParameter("id_localisation", (void*)&localisationId, "int");
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}