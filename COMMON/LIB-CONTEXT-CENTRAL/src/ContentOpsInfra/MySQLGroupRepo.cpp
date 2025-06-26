#include "ContentOpsInfra/MySQLGroupRepo.h"

std::string MySQLGroupRepo::_database = "ice";
std::string MySQLGroupRepo::_table = "groups";

MySQLGroupRepo::MySQLGroupRepo()
{
    _query = nullptr;
}

MySQLGroupRepo::~MySQLGroupRepo()
{
    if (_query != nullptr) {
        delete _query;
    }
}

Query* MySQLGroupRepo::MySQLcreate(COD_Group* group)
{
    _groupIds = group->GetGroupIdPtr();
    if (*_groupIds != -1) { return nullptr; }
    _groupNames = group->GetGroupName();
    _groupParents = &group->GetGroupParentRef();
    
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("name", &_groupNames, "string");
    createQuery->addParameter("id_group_1", _groupParents, "int");
    return createQuery;
}

Query* MySQLGroupRepo::MySQLread(COD_Group* group)
{
    _groupIds = group->GetGroupIdPtr();
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group_1", nullptr, "int");
    if (*_groupIds != -1) {readQuery->addWhereParameter("id_group", _groupIds, "int");}
    return readQuery;
}

Query* MySQLGroupRepo::MySQLread()
{
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group_1", nullptr, "int");
    return readQuery;
}

Query* MySQLGroupRepo::MySQLupdate(COD_Group* group)
{
    _groupIds = group->GetGroupIdPtr();
    if (*_groupIds == -1) { return nullptr; }

    _groupNames = group->GetGroupName();
    _groupParents = &group->GetGroupParentRef();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("name", &_groupNames, "string");
    updateQuery->addParameter("id_group_1", _groupParents, "int");
    updateQuery->addWhereParameter("id_group", _groupIds, "int");
    return updateQuery;
}

Query* MySQLGroupRepo::MySQLremove(COD_Group* group)
{
    _groupIds = group->GetGroupIdPtr();
    if (*_groupIds == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
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

ResultQuery* MySQLGroupRepo::getGroups()
{
    MySQLDBConnection* dbConn = new MySQLDBConnection();
    dbConn->InitConnection();
    Query* query = MySQLread();
    ResultQuery* result = dbConn->ExecuteQuery(query);
    delete query;
    delete dbConn;
    return result;
}

ResultQuery* MySQLGroupRepo::getGroup(int groupId)
{
    MySQLDBConnection* dbConn = new MySQLDBConnection();
    dbConn->InitConnection();
    
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group_1", nullptr, "int");
    readQuery->addWhereParameter("id_group", &groupId, "int");
    
    ResultQuery* result = dbConn->ExecuteQuery(readQuery);
    delete readQuery;
    delete dbConn;
    return result;
} 