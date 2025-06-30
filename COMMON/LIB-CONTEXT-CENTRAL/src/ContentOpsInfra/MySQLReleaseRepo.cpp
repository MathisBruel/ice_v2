#include "ContentOpsInfra/MySQLReleaseRepo.h"
#include "ContentOpsInfra/MySQLDBConnection.h"

std::string MySQLReleaseRepo::_database = "ice";
std::string MySQLReleaseRepo::_table = "releases";

std::unique_ptr<Query> MySQLReleaseRepo::MySQLcreate(COD_Releases* release)
{
    const int* releaseIds = release->GetReleaseId();
    _releaseIds[0] = releaseIds[0];
    _releaseIds[1] = releaseIds[1];
    _releaseIds[2] = releaseIds[2];
    _CPLRefPath = release->GetCPLRefPath();
    if (_releaseIds[0] == -1 || _releaseIds[1] == -1 || _releaseIds[2] == -1) { return nullptr; }
    
    std::unique_ptr<Query> createQuery = std::make_unique<Query>(Query::INSERT, _database, _table);
    createQuery->addParameter("id_content", &_releaseIds[0], "int");
    createQuery->addParameter("id_type", &_releaseIds[1], "int");
    createQuery->addParameter("id_localisation", &_releaseIds[2], "int");
    createQuery->addParameter("release_cpl_ref_path", &_CPLRefPath , "string");
    return createQuery;
}

std::unique_ptr<Query> MySQLReleaseRepo::MySQLread(COD_Releases* release)
{
    const int* releaseIds = release->GetReleaseId();
    _releaseIds[0] = releaseIds[0];
    _releaseIds[1] = releaseIds[1];
    _releaseIds[2] = releaseIds[2];
    
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("id_type",nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("release_cis_path",nullptr, "string");
    readQuery->addParameter("release_cpl_ref_path", nullptr, "string");
    if (*&_releaseIds[0] != -1 ) {readQuery->addWhereParameter("id_content", &_releaseIds[0], "int");};
    if (*&_releaseIds[1] != -1 ) {readQuery->addWhereParameter("id_type", &_releaseIds[1], "int");};
    if (*&_releaseIds[2] != -1 ) {readQuery->addWhereParameter("id_localisation", &_releaseIds[2], "int");};

    return readQuery;
}

std::unique_ptr<Query> MySQLReleaseRepo::MySQLupdate(COD_Releases* release)
{
    const int* releaseIds = release->GetReleaseId();
    _releaseIds[0] = releaseIds[0];
    _releaseIds[1] = releaseIds[1];
    _releaseIds[2] = releaseIds[2];
    _CPLRefPath = release->GetCPLRefPath();


    if (_releaseIds[0] == -1 || _releaseIds[1] == -1 || _releaseIds[2] == -1) { return nullptr; }

    std::unique_ptr<Query> updateQuery = std::make_unique<Query>(Query::UPDATE, _database, _table);

    updateQuery->addParameter("release_cpl_ref_path", &_CPLRefPath, "string");
    updateQuery->addWhereParameter("id_content", &_releaseIds[0], "int");
    updateQuery->addWhereParameter("id_type", &_releaseIds[1], "int");
    updateQuery->addWhereParameter("id_localisation", &_releaseIds[2], "int");

    return updateQuery;
}

std::unique_ptr<Query> MySQLReleaseRepo::MySQLremove(COD_Releases* release)
{
    const int* releaseIds = release->GetReleaseId();
    _releaseIds[0] = releaseIds[0];
    _releaseIds[1] = releaseIds[1];
    _releaseIds[2] = releaseIds[2];
    
    if (_releaseIds[0] == -1 || _releaseIds[1] == -1 || _releaseIds[2] == -1) { return nullptr; }

    std::unique_ptr<Query> removeQuery = std::make_unique<Query>(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_content", &_releaseIds[0], "int");
    removeQuery->addWhereParameter("id_type", &_releaseIds[1], "int");
    removeQuery->addWhereParameter("id_localisation", &_releaseIds[2], "int");
    return removeQuery;
}

// Implémentation des méthodes virtuelles pures
std::unique_ptr<ResultQuery> MySQLReleaseRepo::getReleases()
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("id_localisation", nullptr, "int");
    query->addParameter("release_cis_path", nullptr, "string");
    query->addParameter("release_cpl_ref_path", nullptr, "string");
    query->addParameter("type_name", nullptr, "string");
    query->addParameter("localisation_name", nullptr, "string");
    
    std::string customSQL = "SELECT r.id_content, r.id_type, r.id_localisation, r.release_cis_path, r.release_cpl_ref_path, "
                           "t.name as type_name, l.name as localisation_name "
                           "FROM " + _database + "." + _table + " r "
                           "LEFT JOIN " + _database + ".type t ON r.id_type = t.id_type "
                           "LEFT JOIN " + _database + ".localisation l ON r.id_localisation = l.id_localisation";
    query->setCustomSQL(customSQL);
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLReleaseRepo::getReleases(int contentId)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("id_localisation", nullptr, "int");
    query->addParameter("release_cis_path", nullptr, "string");
    query->addParameter("release_cpl_ref_path", nullptr, "string");
    query->addParameter("type_name", nullptr, "string");
    query->addParameter("localisation_name", nullptr, "string");
    
    std::string customSQL = "SELECT r.id_content, r.id_type, r.id_localisation, r.release_cis_path, r.release_cpl_ref_path, "
                           "t.name as type_name, l.name as localisation_name "
                           "FROM " + _database + "." + _table + " r "
                           "LEFT JOIN " + _database + ".type t ON r.id_type = t.id_type "
                           "LEFT JOIN " + _database + ".localisation l ON r.id_localisation = l.id_localisation "
                           "WHERE r.id_content = " + std::to_string(contentId);
    query->setCustomSQL(customSQL);
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLReleaseRepo::getRelease(int contentId, int typeId, int localisationId)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::CUSTOM, _database, _table);
    
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("id_type", nullptr, "int");
    query->addParameter("id_localisation", nullptr, "int");
    query->addParameter("release_cis_path", nullptr, "string");
    query->addParameter("release_cpl_ref_path", nullptr, "string");
    query->addParameter("type_name", nullptr, "string");
    query->addParameter("localisation_name", nullptr, "string");
    
    std::string customSQL = "SELECT r.id_content, r.id_type, r.id_localisation, r.release_cis_path, r.release_cpl_ref_path, "
                           "t.name as type_name, l.name as localisation_name "
                           "FROM " + _database + "." + _table + " r "
                           "LEFT JOIN " + _database + ".type t ON r.id_type = t.id_type "
                           "LEFT JOIN " + _database + ".localisation l ON r.id_localisation = l.id_localisation "
                           "WHERE r.id_content = " + std::to_string(contentId) + 
                           " AND r.id_type = " + std::to_string(typeId) + 
                           " AND r.id_localisation = " + std::to_string(localisationId);
    query->setCustomSQL(customSQL);
    
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}