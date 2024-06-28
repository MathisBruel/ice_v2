#include "Infra/MySQLSyncLoopRepo.h"

std::string MySQLSyncLoopRepo::_database = "ice";
std::string MySQLSyncLoopRepo::_table = "LoopSync";

Query* MySQLSyncLoopRepo::MySQLcreate(SyncLoop* syncloop){
    int* syncloopIds = syncloop->GetSyncLoopId();
    if (syncloopIds[0] == -1 || syncloopIds[1] == -1 || syncloopIds[2] == -1 || syncloopIds[3] == -1) { return nullptr; }

    std::string syncLoopPath = syncloop->GetSyncLoopPath();

    Query* createQuery = new Query(Query::INSERT, _database, _table);

    createQuery->addParameter("id_serv_pair_config", &syncloopIds[0], "int");
    createQuery->addParameter("id_movie", &syncloopIds[1], "int");
    createQuery->addParameter("id_type", &syncloopIds[2], "int");
    createQuery->addParameter("id_localisation", &syncloopIds[3], "int");
    createQuery->addParameter("LoopSync_path", &syncLoopPath, "string");
    return createQuery;
}
Query* MySQLSyncLoopRepo::MySQLread(SyncLoop* syncloop){
    int* syncloopIds = syncloop->GetSyncLoopId();
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    
    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_movie", nullptr, "int");
    readQuery->addParameter("id_type",nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("LoopSync_path", nullptr, "string");
    if (&syncloopIds[0] != nullptr) { readQuery->addWhereParameter("id_serv_pair_config", &syncloopIds[0], "int"); };
    if (&syncloopIds[1] != nullptr) { readQuery->addWhereParameter("id_movie", &syncloopIds[1], "int"); };
    if (&syncloopIds[2] != nullptr) { readQuery->addWhereParameter("id_type", &syncloopIds[2], "int"); };
    if (&syncloopIds[3] != nullptr) { readQuery->addWhereParameter("id_localisation", &syncloopIds[3], "int"); };

    return readQuery;
}
Query* MySQLSyncLoopRepo::MySQLupdate(SyncLoop* syncloop){
    int* syncloopIds = syncloop->GetSyncLoopId();
    if (syncloopIds[0] == -1 || syncloopIds[1] == -1 || syncloopIds[2] == -1 || syncloopIds[3] == -1) { return nullptr; }

    std::string syncLoopPath = syncloop->GetSyncLoopPath();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("LoopSync_path", &syncLoopPath, "string");
    updateQuery->addWhereParameter("id_serv_pair_config", &syncloopIds[0], "int");
    updateQuery->addWhereParameter("id_movie", &syncloopIds[1], "int");
    updateQuery->addWhereParameter("id_type", &syncloopIds[2], "int");
    updateQuery->addWhereParameter("id_localisation", &syncloopIds[3], "int");
    return updateQuery;
}
Query* MySQLSyncLoopRepo::MySQLremove(SyncLoop* syncloop){
    int* syncloopIds = syncloop->GetSyncLoopId();
    if (syncloopIds[0] == -1 || syncloopIds[1] == -1 || syncloopIds[2] == -1 || syncloopIds[3] == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_serv_pair_config", &syncloopIds[0], "int");
    removeQuery->addWhereParameter("id_movie", &syncloopIds[1], "int");
    removeQuery->addWhereParameter("id_type", &syncloopIds[2], "int");
    removeQuery->addWhereParameter("id_localisation", &syncloopIds[3], "int");
    return removeQuery;
}