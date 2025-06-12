#include "Infra/MySQLSyncLoopRepo.h"

std::string MySQLSyncLoopRepo::_database = "ice";
std::string MySQLSyncLoopRepo::_table = "loopSync";

Query* MySQLSyncLoopRepo::MySQLcreate(SyncLoop* syncloop){
    _syncloopIds = syncloop->GetSyncLoopId();
    if (_syncloopIds[0] == -1 || _syncloopIds[1] == -1 || _syncloopIds[2] == -1 || _syncloopIds[3] == -1) { return nullptr; }

    _syncLoopPath = syncloop->GetSyncLoopPath();

    Query* createQuery = new Query(Query::INSERT, _database, _table);

    createQuery->addParameter("id_serv_pair_config", &_syncloopIds[0], "int");
    createQuery->addParameter("id_content", &_syncloopIds[1], "int");
    createQuery->addParameter("id_type", &_syncloopIds[2], "int");
    createQuery->addParameter("id_localisation", &_syncloopIds[3], "int");
    createQuery->addParameter("path_sync_loop", &_syncLoopPath, "string");
    return createQuery;
}
Query* MySQLSyncLoopRepo::MySQLread(SyncLoop* syncloop){
    _syncloopIds = syncloop->GetSyncLoopId();
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    
    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("id_type",nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("path_sync_loop", nullptr, "string");
    if (*&_syncloopIds[0] != -1 ) { readQuery->addWhereParameter("id_serv_pair_config", &_syncloopIds[0], "int"); };
    if (*&_syncloopIds[1] != -1 ) { readQuery->addWhereParameter("id_content", &_syncloopIds[1], "int"); };
    if (*&_syncloopIds[2] != -1 ) { readQuery->addWhereParameter("id_type", &_syncloopIds[2], "int"); };
    if (*&_syncloopIds[3] != -1 ) { readQuery->addWhereParameter("id_localisation", &_syncloopIds[3], "int"); };

    return readQuery;
}
Query* MySQLSyncLoopRepo::MySQLupdate(SyncLoop* syncloop){
    _syncloopIds = syncloop->GetSyncLoopId();
    if (_syncloopIds[0] == -1 || _syncloopIds[1] == -1 || _syncloopIds[2] == -1 || _syncloopIds[3] == -1) { return nullptr; }

    _syncLoopPath = syncloop->GetSyncLoopPath();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("path_sync_loop", &_syncLoopPath, "string");
    updateQuery->addWhereParameter("id_serv_pair_config", &_syncloopIds[0], "int");
    updateQuery->addWhereParameter("id_content", &_syncloopIds[1], "int");
    updateQuery->addWhereParameter("id_type", &_syncloopIds[2], "int");
    updateQuery->addWhereParameter("id_localisation", &_syncloopIds[3], "int");
    return updateQuery;
}
Query* MySQLSyncLoopRepo::MySQLremove(SyncLoop* syncloop){
    _syncloopIds = syncloop->GetSyncLoopId();
    if (_syncloopIds[0] == -1 || _syncloopIds[1] == -1 || _syncloopIds[2] == -1 || _syncloopIds[3] == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_serv_pair_config", &_syncloopIds[0], "int");
    removeQuery->addWhereParameter("id_content", &_syncloopIds[1], "int");
    removeQuery->addWhereParameter("id_type", &_syncloopIds[2], "int");
    removeQuery->addWhereParameter("id_localisation", &_syncloopIds[3], "int");
    return removeQuery;
}