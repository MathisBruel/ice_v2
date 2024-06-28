#include "Infra/MySQLSyncRepo.h"

std::string MySQLSyncRepo::_database = "ice";
std::string MySQLSyncRepo::_table = "sync";

Query* MySQLSyncRepo::MySQLcreate(Sync* sync)
{
    int* syncId = sync->GetSyncID();
    if (syncId[0] == -1 || syncId[1] == -1 || syncId[2] == -1 || syncId[3] == -1) { return nullptr; }
    std::string syncPath = sync->GetSyncPath();
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("id_serv_pair_config", &syncId[0], "int");
    createQuery->addParameter("id_movie", &syncId[1], "int");
    createQuery->addParameter("id_type", &syncId[2], "int");
    createQuery->addParameter("id_localisation", &syncId[3], "int");
    createQuery->addParameter("sync_path", &syncPath, "string");
    return createQuery;
}
Query* MySQLSyncRepo::MySQLread(Sync* sync)
{
    int* syncId = sync->GetSyncID();
    Query* readQuery = new Query(Query::SELECT, _database, _table);

    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_movie", nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("sync_path", nullptr, "string");
    if (&syncId[0] != nullptr) { readQuery->addWhereParameter("id_serv_pair_config", &syncId[0], "int"); };
    if (&syncId[1] != nullptr) { readQuery->addWhereParameter("id_movie", &syncId[1], "int"); };
    if (&syncId[2] != nullptr) { readQuery->addWhereParameter("id_type", &syncId[2], "int"); };
    if (&syncId[3] != nullptr) { readQuery->addWhereParameter("id_localisation", &syncId[3], "int"); };

    return readQuery;
}
Query* MySQLSyncRepo::MySQLupdate(Sync* sync)
{
    int* syncId = sync->GetSyncID();
    if (syncId[0] == -1 || syncId[1] == -1 || syncId[2] == -1 || syncId[3] == -1) { return nullptr; }
    std::string syncPath = sync->GetSyncPath();
    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("sync_path", &syncPath, "string");
    updateQuery->addWhereParameter("id_serv_pair_config", &syncId[0], "int");
    updateQuery->addWhereParameter("id_movie", &syncId[1], "int");
    updateQuery->addWhereParameter("id_type", &syncId[2], "int");
    updateQuery->addWhereParameter("id_localisation", &syncId[3], "int");
    return updateQuery;
}
Query* MySQLSyncRepo::MySQLremove(Sync* sync)
{
    int* syncId = sync->GetSyncID();
    if (syncId[0] == -1 || syncId[1] == -1 || syncId[2] == -1 || syncId[3] == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_serv_pair_config", &syncId[0], "int");
    removeQuery->addWhereParameter("id_movie", &syncId[1], "int");
    removeQuery->addWhereParameter("id_type", &syncId[2], "int");
    removeQuery->addWhereParameter("id_localisation", &syncId[3], "int");
    return removeQuery;
}