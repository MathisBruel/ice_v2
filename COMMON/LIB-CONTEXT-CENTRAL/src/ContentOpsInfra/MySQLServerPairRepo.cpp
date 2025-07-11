#include "ContentOpsInfra/MySQLServerPairRepo.h"
#include "ContentOpsDomain/COD_ServerPair.h"

MySQLServerPairRepo::MySQLServerPairRepo()
    : _connection(nullptr), _ownConnection(true)
{
    _connection = new MySQLDBConnection();
}

MySQLServerPairRepo::MySQLServerPairRepo(MySQLDBConnection* connection)
    : _connection(connection), _ownConnection(false)
{
}

MySQLServerPairRepo::~MySQLServerPairRepo()
{
    if (_ownConnection && _connection) {
        delete _connection;
    }
}

void MySQLServerPairRepo::Create(COD_ServerPair* serverPair)
{
    if (!serverPair || !_connection) return;

    Query* query = new Query(Query::INSERT, "ice", "server_pair_configuration");
    query->addParameter("projection_server_ip", serverPair->GetProjectionServerIpPtr(), "string");
    query->addParameter("auditorium_server_ip", serverPair->GetAuditoriumServerIpPtr(), "string");
    query->addParameter("name", serverPair->GetNamePtr(), "string");
    query->addParameter("id_default_auditorium", serverPair->GetIdDefaultAuditoriumPtr(), "int");
    query->addParameter("id_site", serverPair->GetIdSitePtr(), "int");

    auto result = std::unique_ptr<ResultQuery>(_connection->ExecuteQuery(query));
    
    if (result && result->isValid() && result->getLastInsertedId() > 0) {
        serverPair->SetServerPairId(static_cast<int>(result->getLastInsertedId()));
    }

    delete query;
}

void MySQLServerPairRepo::Read(COD_ServerPair* serverPair)
{
    if (!serverPair || !_connection) return;

    Query* query = new Query(Query::SELECT, "ice", "server_pair_configuration");
    query->addParameter("id_serv_pair_config", nullptr, "int");
    query->addParameter("projection_server_ip", nullptr, "string");
    query->addParameter("auditorium_server_ip", nullptr, "string");
    query->addParameter("name", nullptr, "string");
    query->addParameter("id_default_auditorium", nullptr, "int");
    query->addParameter("id_site", nullptr, "int");
    query->addWhereParameter("id_serv_pair_config", serverPair->GetServerPairIdPtr(), "int");

    auto result = std::unique_ptr<ResultQuery>(_connection->ExecuteQuery(query));
    
    if (result && result->isValid() && result->getNbRows() > 0) {
        serverPair->SetDatas(
            *result->getStringValue(0, "projection_server_ip"),
            *result->getStringValue(0, "auditorium_server_ip"),
            *result->getStringValue(0, "name"),
            *result->getIntValue(0, "id_default_auditorium"),
            *result->getIntValue(0, "id_site")
        );
    }

    delete query;
}

void MySQLServerPairRepo::Update(COD_ServerPair* serverPair)
{
    if (!serverPair || !_connection) return;

    Query* query = new Query(Query::UPDATE, "ice", "server_pair_configuration");
    query->addParameter("projection_server_ip", serverPair->GetProjectionServerIpPtr(), "string");
    query->addParameter("auditorium_server_ip", serverPair->GetAuditoriumServerIpPtr(), "string");
    query->addParameter("name", serverPair->GetNamePtr(), "string");
    query->addParameter("id_default_auditorium", serverPair->GetIdDefaultAuditoriumPtr(), "int");
    query->addParameter("id_site", serverPair->GetIdSitePtr(), "int");
    query->addWhereParameter("id_serv_pair_config", serverPair->GetServerPairIdPtr(), "int");

    auto result = std::unique_ptr<ResultQuery>(_connection->ExecuteQuery(query));

    delete query;
}

void MySQLServerPairRepo::Remove(COD_ServerPair* serverPair)
{
    if (!serverPair || !_connection) return;

    Query* query = new Query(Query::REMOVE, "ice", "server_pair_configuration");
    query->addWhereParameter("id_serv_pair_config", serverPair->GetServerPairIdPtr(), "int");

    auto result = std::unique_ptr<ResultQuery>(_connection->ExecuteQuery(query));

    delete query;
}

std::unique_ptr<ResultQuery> MySQLServerPairRepo::getServerPairs()
{
    if (!_connection) return nullptr;

    Query* query = new Query(Query::SELECT, "ice", "server_pair_configuration");
    query->addParameter("id_serv_pair_config", nullptr, "int");
    query->addParameter("projection_server_ip", nullptr, "string");
    query->addParameter("auditorium_server_ip", nullptr, "string");
    query->addParameter("name", nullptr, "string");
    query->addParameter("id_default_auditorium", nullptr, "int");
    query->addParameter("id_site", nullptr, "int");

    auto result = std::unique_ptr<ResultQuery>(_connection->ExecuteQuery(query));
    delete query;
    
    return result;
}

std::unique_ptr<ResultQuery> MySQLServerPairRepo::getServerPair(int id_serv_pair_config)
{
    if (!_connection) return nullptr;

    Query* query = new Query(Query::SELECT, "ice", "server_pair_configuration");
    query->addParameter("id_serv_pair_config", nullptr, "int");
    query->addParameter("projection_server_ip", nullptr, "string");
    query->addParameter("auditorium_server_ip", nullptr, "string");
    query->addParameter("name", nullptr, "string");
    query->addParameter("id_default_auditorium", nullptr, "int");
    query->addParameter("id_site", nullptr, "int");
    
    int* idPtr = const_cast<int*>(&id_serv_pair_config);
    query->addWhereParameter("id_serv_pair_config", idPtr, "int");

    auto result = std::unique_ptr<ResultQuery>(_connection->ExecuteQuery(query));
    delete query;
    
    return result;
}

std::unique_ptr<ResultQuery> MySQLServerPairRepo::getServerPairsBySite(int id_site)
{
    if (!_connection) return nullptr;

    Query* query = new Query(Query::SELECT, "ice", "server_pair_configuration");
    query->addParameter("id_serv_pair_config", nullptr, "int");
    query->addParameter("projection_server_ip", nullptr, "string");
    query->addParameter("auditorium_server_ip", nullptr, "string");
    query->addParameter("name", nullptr, "string");
    query->addParameter("id_default_auditorium", nullptr, "int");
    query->addParameter("id_site", nullptr, "int");
    
    int* siteIdPtr = const_cast<int*>(&id_site);
    query->addWhereParameter("id_site", siteIdPtr, "int");

    auto result = std::unique_ptr<ResultQuery>(_connection->ExecuteQuery(query));
    delete query;
    
    return result;
} 