#include "ContentOpsInfra/MySQLSiteRepo.h"
#include "Poco/Logger.h"

std::string MySQLSiteRepo::_database = "ice";
std::string MySQLSiteRepo::_table = "site";

MySQLSiteRepo::MySQLSiteRepo()
{
    _query = nullptr;
}

MySQLSiteRepo::~MySQLSiteRepo()
{
}

std::unique_ptr<Query> MySQLSiteRepo::MySQLcreate(COD_Site* site)
{
    _siteIds = site->GetSiteIdPtr();
    if (*_siteIds != -1) { return nullptr; }
    _siteNames = site->GetSiteName();
    _siteGroups = &site->GetSiteGroupRef();
    _siteConnections = &site->GetSiteConnectionRef();
    
    std::unique_ptr<Query> createQuery = std::make_unique<Query>(Query::INSERT, _database, _table);
    createQuery->addParameter("name", &_siteNames, "string");
    createQuery->addParameter("id_group", _siteGroups, "int");
    createQuery->addParameter("id_connection", _siteConnections, "int");
    return createQuery;
}

std::unique_ptr<Query> MySQLSiteRepo::MySQLread(COD_Site* site)
{
    _siteIds = site->GetSiteIdPtr();
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_site", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("id_connection", nullptr, "int");
    if (*_siteIds != -1) {readQuery->addWhereParameter("id_site", _siteIds, "int");}
    return readQuery;
}

std::unique_ptr<Query> MySQLSiteRepo::MySQLread(int* groupId)
{
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_site", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("id_connection", nullptr, "int");
    if (groupId != nullptr) {
        readQuery->addWhereParameter("id_group", groupId, "int");
    }
    return readQuery;
}

std::unique_ptr<Query> MySQLSiteRepo::MySQLupdate(COD_Site* site)
{
    _siteIds = site->GetSiteIdPtr();
    if (*_siteIds == -1) { return nullptr; }

    _siteNames = site->GetSiteName();
    _siteGroups = &site->GetSiteGroupRef();
    _siteConnections = &site->GetSiteConnectionRef();

    std::unique_ptr<Query> updateQuery = std::make_unique<Query>(Query::UPDATE, _database, _table);
    updateQuery->addParameter("name", &_siteNames, "string");
    updateQuery->addParameter("id_group", _siteGroups, "int");
    updateQuery->addParameter("id_connection", _siteConnections, "int");
    updateQuery->addWhereParameter("id_site", _siteIds, "int");
    return updateQuery;
}

std::unique_ptr<Query> MySQLSiteRepo::MySQLremove(COD_Site* site)
{
    _siteIds = site->GetSiteIdPtr();
    if (*_siteIds == -1) { return nullptr; }

    std::unique_ptr<Query> removeQuery = std::make_unique<Query>(Query::REMOVE, _database, _table);
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

std::unique_ptr<ResultQuery> MySQLSiteRepo::getSites(int groupId)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::string sql = R"(
        WITH RECURSIVE group_hierarchy AS (
            SELECT id_group, id_group_1, name, 0 as level
            FROM `groups` 
            WHERE id_group = ?
            
            UNION ALL
            
            SELECT g.id_group, g.id_group_1, g.name, gh.level + 1
            FROM `groups` g
            INNER JOIN group_hierarchy gh ON g.id_group_1 = gh.id_group
        )
        SELECT DISTINCT s.id_site, s.name, s.id_group, s.id_connection, gh.level
        FROM site s
        INNER JOIN group_hierarchy gh ON s.id_group = gh.id_group
        ORDER BY gh.level, s.name
    )";
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, "");
    query->setCustomSQL(sql);

    query->addParameter("id_site", nullptr, "int");
    query->addParameter("name", nullptr, "string");
    query->addParameter("id_group", nullptr, "int");
    query->addParameter("id_connection", nullptr, "int");
    query->addParameter("level", nullptr, "int");
    
    
    query->addParameter("groupId", &groupId, "int");
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

