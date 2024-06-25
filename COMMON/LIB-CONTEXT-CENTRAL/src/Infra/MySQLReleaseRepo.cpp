#include "MySQLReleaseRepo.h"

std::string MySQLReleaseRepo::database = "ice";
std::string MySQLReleaseRepo::table = "movie";

Query* MySQLReleaseRepo::create(Release* release)
{
    const int* releaseIds = release->getId();
    if (releaseIds[0] == -1 || releaseIds[1] == -1 || releaseIds[2] == -1) { return nullptr; }
    
    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("id_movie", &releaseIds[0], "int");
    createQuery->addParameter("id_type", &releaseIds[1], "int");
    createQuery->addParameter("id_localisation", &releaseIds[2], "int");
    createQuery->addParameter("release_cis_path", nullptr , "string");
    return createQuery;
}

Query* MySQLReleaseRepo::read(Release* release)
{
    const int* releaseIds = release->getId();
    Query* readQuery = new Query(Query::SELECT, database, table);
    readQuery->addParameter("id_movie", nullptr, "int");
    readQuery->addParameter("id_type",nullptr, "int");
    readQuery->addParameter("id_localisation", nullptr, "int");
    readQuery->addParameter("release_cis_path",nullptr, "string");
    if (&releaseIds[0] != nullptr) {readQuery->addWhereParameter("id_movie", &releaseIds[0], "int");};
    if (&releaseIds[1] != nullptr) {readQuery->addWhereParameter("id_type", &releaseIds[1], "int");};
    if (&releaseIds[2] != nullptr) {readQuery->addWhereParameter("id_localisation", &releaseIds[2], "int");};

    return readQuery;
}

Query* MySQLReleaseRepo::update(Release* release)
{
    const int* releaseIds = release->getId();

    if (releaseIds[0] == -1 || releaseIds[1] == -1 || releaseIds[2] == -1) { return nullptr; }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addWhereParameter("id_movie", &releaseIds[0], "int");
    updateQuery->addWhereParameter("id_type", &releaseIds[1], "int");
    updateQuery->addWhereParameter("id_localisation", &releaseIds[2], "int");
    return updateQuery;
}

Query* MySQLReleaseRepo::remove(Release* release)
{
    if (releaseIds[0] == -1 || releaseIds[1] == -1 || releaseIds[2] == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, database, table);
    removeQuery->addWhereParameter("id_movie", &releaseIds[0], "int");
    removeQuery->addWhereParameter("id_type", &releaseIds[1], "int");
    removeQuery->addWhereParameter("id_localisation", &releaseIds[2], "int");
    return removeQuery;
}