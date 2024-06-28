#include "Infra/MySQLContentRepo.h"

std::string MySQLContentRepo::_database = "ice";
std::string MySQLContentRepo::_table = "movie";

Query* MySQLContentRepo::MySQLcreate(Content* content)
{
    int* id = content->GetContentId();
    if (*id != -1) { return nullptr; }
    std::string title = content->GetContentTitle();
    
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("movie_title", &title, "string");
    return createQuery;
}

Query* MySQLContentRepo::MySQLread(Content* content)
{
    int* id = content->GetContentId();
    std::string title = content->GetContentTitle();

    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id", nullptr, "int");
    readQuery->addParameter("movie_title", nullptr, "string");
    if (id != nullptr) {readQuery->addWhereParameter("id", id, "int");}
    return readQuery;
}

Query* MySQLContentRepo::MySQLupdate(Content* content)
{
    int* id = content->GetContentId();
    if (*id == -1) { return nullptr; }

    std::string title = content->GetContentTitle();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("movie_title", &id, "string");
    updateQuery->addWhereParameter("id", &title, "int");
    return updateQuery;
}

Query* MySQLContentRepo::MySQLremove(Content* content)
{
    int* id = content->GetContentId();
    if (*id == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id", &id, "int");
    return removeQuery;
}