#include "ContentOpsInfra/MySQLSiteRepo.h"

#include "ContentOpsDomain/COD_Site.h"

std::string MySQLSiteRepo::_database = "ice";
std::string MySQLSiteRepo::_table = "site";

Query* MySQLSiteRepo::MySQLcreate(COD_Site* site)
{
    _siteIds = site->GetSiteId();
    if (*_siteIds != -1) { return nullptr; }
    _siteNames = site->GetSiteName();
    _siteGroups = &site->GetSiteGroup();
    _siteConnections = &site->GetSiteConnection();
    
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("name", &_siteNames, "string");
    createQuery->addParameter("id_group", _siteGroups, "int");
    createQuery->addParameter("id_connection", _siteConnections, "int");
    return createQuery;
}

Query* MySQLSiteRepo::MySQLread(COD_Site* site)
{
    _siteIds = site->GetSiteId();
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_site", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("id_connection", nullptr, "int");
    if (*_siteIds != -1) {readQuery->addWhereParameter("id_site", _siteIds, "int");}
    return readQuery;
}

Query* MySQLSiteRepo::MySQLread()
{
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_site", nullptr, "int");
    readQuery->addParameter("name", nullptr, "string");
    readQuery->addParameter("id_group", nullptr, "int");
    readQuery->addParameter("id_connection", nullptr, "int");
    return readQuery;
}

Query* MySQLSiteRepo::MySQLupdate(COD_Site* site)
{
    _siteIds = site->GetSiteId();
    if (*_siteIds == -1) { return nullptr; }

    _siteNames = site->GetSiteName();
    _siteGroups = &site->GetSiteGroup();
    _siteConnections = &site->GetSiteConnection();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("name", &_siteNames, "string");
    updateQuery->addParameter("id_group", _siteGroups, "int");
    updateQuery->addParameter("id_connection", _siteConnections, "int");
    updateQuery->addWhereParameter("id_site", _siteIds, "int");
    return updateQuery;
}

Query* MySQLSiteRepo::MySQLremove(COD_Site* site)
{
    _siteIds = site->GetSiteId();
    if (*_siteIds == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_site", _siteIds, "int");
    return removeQuery;
}

