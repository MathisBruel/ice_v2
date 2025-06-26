#include "ContentOpsInfra/MySQLSiteRepo.h"

std::string MySQLSiteRepo::_database = "ice";
std::string MySQLSiteRepo::_table = "site";

MySQLSiteRepo::MySQLSiteRepo()
{
    _query = nullptr;
}

MySQLSiteRepo::~MySQLSiteRepo()
{
    if (_query != nullptr) {
        delete _query;
    }
}

Query* MySQLSiteRepo::MySQLcreate(COD_Site* site)
{
    _siteIds = site->GetSiteIdPtr();
    if (*_siteIds != -1) { return nullptr; }
    _siteNames = site->GetSiteName();
    _siteGroups = &site->GetSiteGroupRef();
    _siteConnections = &site->GetSiteConnectionRef();
    
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("name", &_siteNames, "string");
    createQuery->addParameter("id_group", _siteGroups, "int");
    createQuery->addParameter("id_connection", _siteConnections, "int");
    return createQuery;
}

Query* MySQLSiteRepo::MySQLread(COD_Site* site)
{
    _siteIds = site->GetSiteIdPtr();
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_site", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("id_connection", nullptr, "int");
    if (*_siteIds != -1) {readQuery->addWhereParameter("id_site", _siteIds, "int");}
    return readQuery;
}

Query* MySQLSiteRepo::MySQLread(int* groupId)
{
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_site", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("id_connection", nullptr, "int");
    if (groupId != nullptr) {
        readQuery->addWhereParameter("id_group", groupId, "int");
    }
    return readQuery;
}

Query* MySQLSiteRepo::MySQLupdate(COD_Site* site)
{
    _siteIds = site->GetSiteIdPtr();
    if (*_siteIds == -1) { return nullptr; }

    _siteNames = site->GetSiteName();
    _siteGroups = &site->GetSiteGroupRef();
    _siteConnections = &site->GetSiteConnectionRef();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("name", &_siteNames, "string");
    updateQuery->addParameter("id_group", _siteGroups, "int");
    updateQuery->addParameter("id_connection", _siteConnections, "int");
    updateQuery->addWhereParameter("id_site", _siteIds, "int");
    return updateQuery;
}

Query* MySQLSiteRepo::MySQLremove(COD_Site* site)
{
    _siteIds = site->GetSiteIdPtr();
    if (*_siteIds == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_site", _siteIds, "int");
    return removeQuery;
}

void MySQLSiteRepo::Create(COD_Site* site) 
{
    _query = MySQLcreate(site);
}

void MySQLSiteRepo::Read(COD_Site* site) 
{
    _query = MySQLread(site);
}

void MySQLSiteRepo::Update(COD_Site* site) 
{
    _query = MySQLupdate(site);
}

void MySQLSiteRepo::Remove(COD_Site* site) 
{
    _query = MySQLremove(site);
}

ResultQuery* MySQLSiteRepo::getSites(int groupId)
{
    MySQLDBConnection* dbConn = new MySQLDBConnection();
    dbConn->InitConnection();
    int* groupIdPtr = new int(groupId);
    Query* query = MySQLread(groupIdPtr);
    ResultQuery* result = dbConn->ExecuteQuery(query);
    delete groupIdPtr;
    delete query;
    delete dbConn;
    return result;
}

