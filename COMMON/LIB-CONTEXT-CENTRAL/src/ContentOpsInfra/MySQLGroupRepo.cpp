#include "ContentOpsInfra/MySQLGroupRepo.h"

std::string MySQLGroupRepo::_database = "ice";
std::string MySQLGroupRepo::_table = "groups";

MySQLGroupRepo::MySQLGroupRepo()
{
    _query = nullptr;
    _dbConnection = nullptr;
}

MySQLGroupRepo::MySQLGroupRepo(std::shared_ptr<MySQLDBConnection> dbConnection)
{
    _query = nullptr;
    _dbConnection = dbConnection;
}

MySQLGroupRepo::~MySQLGroupRepo()
{
}

std::unique_ptr<Query> MySQLGroupRepo::MySQLcreate(COD_Group* group)
{
    _groupIds = group->GetGroupIdPtr();
    if (*_groupIds != -1) { return nullptr; }
    _groupNames = group->GetGroupName();
    _groupParents = &group->GetGroupParentRef();
    
    std::unique_ptr<Query> createQuery = std::make_unique<Query>(Query::INSERT, _database, _table);
    createQuery->addParameter("name", &_groupNames, "string");
    createQuery->addParameter("id_group_1", _groupParents, "int");
    return createQuery;
}

std::unique_ptr<Query> MySQLGroupRepo::MySQLread(COD_Group* group)
{
    _groupIds = group->GetGroupIdPtr();
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group_1", nullptr, "int");
    if (*_groupIds != -1) {readQuery->addWhereParameter("id_group", _groupIds, "int");}
    return readQuery;
}

std::unique_ptr<Query> MySQLGroupRepo::MySQLread()
{
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group_1", nullptr, "int");
    return readQuery;
}

std::unique_ptr<Query> MySQLGroupRepo::MySQLupdate(COD_Group* group)
{
    _groupIds = group->GetGroupIdPtr();
    if (*_groupIds == -1) { return nullptr; }

    _groupNames = group->GetGroupName();
    _groupParents = &group->GetGroupParentRef();

    std::unique_ptr<Query> updateQuery = std::make_unique<Query>(Query::UPDATE, _database, _table);
    updateQuery->addParameter("name", &_groupNames, "string");
    updateQuery->addParameter("id_group_1", _groupParents, "int");
    updateQuery->addWhereParameter("id_group", _groupIds, "int");
    return updateQuery;
}

std::unique_ptr<Query> MySQLGroupRepo::MySQLremove(COD_Group* group)
{
    _groupIds = group->GetGroupIdPtr();
    if (*_groupIds == -1) { return nullptr; }

    std::unique_ptr<Query> removeQuery = std::make_unique<Query>(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_group", _groupIds, "int");
    return removeQuery;
}

void MySQLGroupRepo::Create(COD_Group* group) 
{
    _query = MySQLcreate(group);
}

void MySQLGroupRepo::Read(COD_Group* group) 
{
    _query = MySQLread(group);
}

void MySQLGroupRepo::Update(COD_Group* group) 
{
    _query = MySQLupdate(group);
}

void MySQLGroupRepo::Remove(COD_Group* group) 
{
    _query = MySQLremove(group);
}

std::unique_ptr<ResultQuery> MySQLGroupRepo::getGroups()
{
    if (!_dbConnection) {
        _dbConnection = std::make_shared<MySQLDBConnection>();
        _dbConnection->InitConnection();
    }
    std::unique_ptr<Query> query = MySQLread();
    std::unique_ptr<ResultQuery> result(_dbConnection->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLGroupRepo::getGroup(int groupId)
{
    if (!_dbConnection) {
        _dbConnection = std::make_shared<MySQLDBConnection>();
        _dbConnection->InitConnection();
    }
    
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group_1", nullptr, "int");
    readQuery->addWhereParameter("id_group", &groupId, "int");
    
    std::unique_ptr<ResultQuery> result(_dbConnection->ExecuteQuery(readQuery.get()));
    return result;
} 