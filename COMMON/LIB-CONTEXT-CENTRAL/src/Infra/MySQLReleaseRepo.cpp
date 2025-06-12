#include "Infra/MySQLReleaseRepo.h"

std::string MySQLReleaseRepo::_database = "ice";
std::string MySQLReleaseRepo::_table = "releases";

Query* MySQLReleaseRepo::MySQLcreate(Releases* release)
{
    _releaseIds = release->GetReleaseId();
    _CPLRefPath = release->GetCPLRefPath();
    if (_releaseIds[0] == -1 || _releaseIds[1] == -1 || _releaseIds[2] == -1) { return nullptr; }
    
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("id_content", &_releaseIds[0], "int");
    createQuery->addParameter("id_type", &_releaseIds[1], "int");
    createQuery->addParameter("id_localisation", &_releaseIds[2], "int");
    createQuery->addParameter("release_cpl_ref_path", &_CPLRefPath , "string");
    return createQuery;
}

Query* MySQLReleaseRepo::MySQLread(Releases* release)
{
    _releaseIds = release->GetReleaseId();
    
    Query* readQuery = new Query(Query::SELECT, _database, _table);
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

Query* MySQLReleaseRepo::MySQLupdate(Releases* release)
{
    _releaseIds = release->GetReleaseId();
    _CPLRefPath = release->GetCPLRefPath();


    if (_releaseIds[0] == -1 || _releaseIds[1] == -1 || _releaseIds[2] == -1) { return nullptr; }

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);

    updateQuery->addParameter("release_cpl_ref_path", &_CPLRefPath, "string");
    updateQuery->addWhereParameter("id_content", &_releaseIds[0], "int");
    updateQuery->addWhereParameter("id_type", &_releaseIds[1], "int");
    updateQuery->addWhereParameter("id_localisation", &_releaseIds[2], "int");

    return updateQuery;
}

Query* MySQLReleaseRepo::MySQLremove(Releases* release)
{
    _releaseIds = release->GetReleaseId();
    
    if (_releaseIds[0] == -1 || _releaseIds[1] == -1 || _releaseIds[2] == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_content", &_releaseIds[0], "int");
    removeQuery->addWhereParameter("id_type", &_releaseIds[1], "int");
    removeQuery->addWhereParameter("id_localisation", &_releaseIds[2], "int");
    return removeQuery;
}