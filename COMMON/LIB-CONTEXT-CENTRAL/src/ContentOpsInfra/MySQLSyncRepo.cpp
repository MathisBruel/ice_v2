#include "ContentOpsInfra/MySQLSyncRepo.h"

std::string MySQLSyncRepo::_database = "ice";
std::string MySQLSyncRepo::_table = "sync";

Query* MySQLSyncRepo::MySQLcreate(Sync* sync)
{
    int* _syncId = sync->GetSyncID();
    if (_syncId[0] == -1 || _syncId[1] == -1 || _syncId[2] == -1 || _syncId[3] == -1) { return nullptr; }
    _syncPath = sync->GetSyncPath();
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("id_serv_pair_config", &_syncId[0], "int");
    createQuery->addParameter("id_content", &_syncId[1], "int");
    createQuery->addParameter("id_type", &_syncId[2], "int");
    createQuery->addParameter("id_localisation", &_syncId[3], "int");
    createQuery->addParameter("path_sync", &_syncPath, "string");
    return createQuery;
}
Query* MySQLSyncRepo::MySQLread(Sync* sync)
{
    _syncId = sync->GetSyncID();
    Query* readQuery = new Query(Query::SELECT, _database, _table);

    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("path_sync", nullptr, "string");
    if (*&_syncId[0] != -1 ) { readQuery->addWhereParameter("id_serv_pair_config", &_syncId[0], "int"); };
    if (*&_syncId[1] != -1 ) { readQuery->addWhereParameter("id_content", &_syncId[1], "int"); };
    if (*&_syncId[2] != -1 ) { readQuery->addWhereParameter("id_type", &_syncId[2], "int"); };
    if (*&_syncId[3] != -1 ) { readQuery->addWhereParameter("id_localisation", &_syncId[3], "int"); };

    return readQuery;
}
Query* MySQLSyncRepo::MySQLupdate(Sync* sync)
{
    _syncId = sync->GetSyncID();
    if (_syncId[0] == -1 || _syncId[1] == -1 || _syncId[2] == -1 || _syncId[3] == -1) { return nullptr; }
    _syncPath = sync->GetSyncPath();
    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("path_sync", &_syncPath, "string");
    updateQuery->addWhereParameter("id_serv_pair_config", &_syncId[0], "int");
    updateQuery->addWhereParameter("id_content", &_syncId[1], "int");
    updateQuery->addWhereParameter("id_type", &_syncId[2], "int");
    updateQuery->addWhereParameter("id_localisation", &_syncId[3], "int");
    return updateQuery;
}
Query* MySQLSyncRepo::MySQLremove(Sync* sync)
{
    _syncId = sync->GetSyncID();
    if (_syncId[0] == -1 || _syncId[1] == -1 || _syncId[2] == -1 || _syncId[3] == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_serv_pair_config", &_syncId[0], "int");
    removeQuery->addWhereParameter("id_content", &_syncId[1], "int");
    removeQuery->addWhereParameter("id_type", &_syncId[2], "int");
    removeQuery->addWhereParameter("id_localisation", &_syncId[3], "int");
    return removeQuery;
}