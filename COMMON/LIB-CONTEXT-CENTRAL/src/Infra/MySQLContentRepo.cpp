#include "Infra/MySQLContentRepo.h"

std::string MySQLContentRepo::_database = "ice";
std::string MySQLContentRepo::_table = "movie";

Query* MySQLContentRepo::MySQLcreate(Content* content)
{
    int* id = content->GetContentId();
    if (*id != -1) { return nullptr; }
    std::string title = content->GetContentTitle();
    
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("title", &title, "string");
    return createQuery;
}

Query* MySQLContentRepo::MySQLread(Content* content)
{
    int* id = content->GetContentId();
    std::string title = content->GetContentTitle();

    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_movie", nullptr, "int");
    readQuery->addParameter("title", nullptr, "string");
    if (id != nullptr) {readQuery->addWhereParameter("id_movie", id, "int");}
    return readQuery;
}

Query* MySQLContentRepo::MySQLupdate(Content* content)
{
    int* id = content->GetContentId();
    if (*id == -1) { return nullptr; }

    std::string title = content->GetContentTitle();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("title", &id, "string");
    updateQuery->addWhereParameter("id_movie", &title, "int");
    return updateQuery;
}

Query* MySQLContentRepo::MySQLremove(Content* content)
{
    int* id = content->GetContentId();
    if (*id == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_movie", &id, "int");
    return removeQuery;
}