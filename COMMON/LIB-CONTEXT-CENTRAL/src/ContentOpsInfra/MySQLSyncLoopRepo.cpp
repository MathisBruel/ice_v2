#include "ContentOpsInfra/MySQLSyncLoopRepo.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "ContentOpsBoundary/COB_SyncLoop.h"
#include <memory>
#include <iostream> 
std::string MySQLSyncLoopRepo::_database = "ice";
std::string MySQLSyncLoopRepo::_table = "loopSync";

MySQLSyncLoopRepo::MySQLSyncLoopRepo() {
    _query = nullptr;
}

MySQLSyncLoopRepo::~MySQLSyncLoopRepo() {}

std::unique_ptr<Query> MySQLSyncLoopRepo::MySQLcreate(COB_SyncLoop* syncLoop) {
    if (!syncLoop) return nullptr;
    const int* ids = syncLoop->GetSyncLoopId();
    int* id_serv_pair_config = new int(ids[0]);
    int* id_content = new int(ids[1]);
    int* id_type = new int(ids[2]);
    int* id_localisation = new int(ids[3]);
    std::string* path = new std::string(syncLoop->GetSyncLoopPath());
    
    std::unique_ptr<Query> createQuery = std::make_unique<Query>(Query::INSERT, _database, _table);
    createQuery->addParameter("id_serv_pair_config", id_serv_pair_config, "int");
    createQuery->addParameter("id_content", id_content, "int");
    createQuery->addParameter("id_type", id_type, "int");
    createQuery->addParameter("id_localisation", id_localisation, "int");
    createQuery->addParameter("path_sync_loop", path, "string");
    return createQuery;
}

std::unique_ptr<Query> MySQLSyncLoopRepo::MySQLread(COB_SyncLoop* syncLoop) {
    if (!syncLoop) return nullptr;
    const int* ids = syncLoop->GetSyncLoopId();
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("path_sync_loop", nullptr, "string");
    readQuery->addWhereParameter("id_serv_pair_config", (void*)&ids[0], "int");
    readQuery->addWhereParameter("id_content", (void*)&ids[1], "int");
    readQuery->addWhereParameter("id_type", (void*)&ids[2], "int");
    readQuery->addWhereParameter("id_localisation", (void*)&ids[3], "int");
    return readQuery;
}

std::unique_ptr<Query> MySQLSyncLoopRepo::MySQLread() {
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_serv_pair_config", nullptr, "int");
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("id_type", nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("path_sync_loop", nullptr, "string");
    return readQuery;
}

std::unique_ptr<Query> MySQLSyncLoopRepo::MySQLupdate(COB_SyncLoop* syncLoop) {
    if (!syncLoop) return nullptr;
    const int* ids = syncLoop->GetSyncLoopId();
    
    int* id_serv_pair_config = new int(ids[0]);
    int* id_content = new int(ids[1]);
    int* id_type = new int(ids[2]);
    int* id_localisation = new int(ids[3]);
    std::string* path = new std::string(syncLoop->GetSyncLoopPath());
    
    std::unique_ptr<Query> updateQuery = std::make_unique<Query>(Query::UPDATE, _database, _table);
    updateQuery->addParameter("path_sync_loop", path, "string");
    updateQuery->addWhereParameter("id_serv_pair_config", id_serv_pair_config, "int");
    updateQuery->addWhereParameter("id_content", id_content, "int");
    updateQuery->addWhereParameter("id_type", id_type, "int");
    updateQuery->addWhereParameter("id_localisation", id_localisation, "int");
    
    return updateQuery;
}

std::unique_ptr<Query> MySQLSyncLoopRepo::MySQLremove(COB_SyncLoop* syncLoop) {
    if (!syncLoop) return nullptr;
    const int* ids = syncLoop->GetSyncLoopId();
    std::unique_ptr<Query> removeQuery = std::make_unique<Query>(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_serv_pair_config", (void*)&ids[0], "int");
    removeQuery->addWhereParameter("id_content", (void*)&ids[1], "int");
    removeQuery->addWhereParameter("id_type", (void*)&ids[2], "int");
    removeQuery->addWhereParameter("id_localisation", (void*)&ids[3], "int");
    return removeQuery;
}

std::unique_ptr<ResultQuery> MySQLSyncLoopRepo::getSyncLoops() {
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::unique_ptr<Query> query = MySQLread();
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLSyncLoopRepo::getSyncLoopsByRelease(int contentId, int typeId, int localisationId) {
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::SELECT, _database, _table);
    query->addParameter("id_serv_pair_config", nullptr, "int");
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("id_localisation", nullptr, "int");
    query->addParameter("path_sync_loop", nullptr, "string");
    query->addWhereParameter("id_content", &contentId, "int");
    query->addWhereParameter("id_type", &typeId, "int");
    query->addWhereParameter("id_localisation", &localisationId, "int");
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

void MySQLSyncLoopRepo::Create(COB_SyncLoop* syncLoop) {
    _query = MySQLcreate(syncLoop);
    if (_query) {
        std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
        dbConn->InitConnection();
        dbConn->ExecuteQuery(_query.get());
    }
}

void MySQLSyncLoopRepo::Remove(COB_SyncLoop* syncLoop) {
    _query = MySQLremove(syncLoop);
    if (_query) {
        std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
        dbConn->InitConnection();
        dbConn->ExecuteQuery(_query.get());
    }
}

void MySQLSyncLoopRepo::Update(COB_SyncLoop* syncLoop) {
    _query = MySQLupdate(syncLoop);
    if (_query) {
        std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
        dbConn->InitConnection();   
        dbConn->ExecuteQuery(_query.get());
    }
}

