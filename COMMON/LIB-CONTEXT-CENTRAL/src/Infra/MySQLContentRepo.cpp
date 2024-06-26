#include "Infra/MySQLContentRepo.h"

std::string MySQLContentRepo::database = "ice";
std::string MySQLContentRepo::table = "movie";

Query* MySQLContentRepo::MySQLcreate(Content* content)
{
    int* id = content->getId();
    if (*id != -1) { return nullptr; }
    std::string title = content->getTitle();
    
    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("movie_title", &title, "string");
    return createQuery;
}

Query* MySQLContentRepo::MySQLread(Content* content)
{
    int* id = content->getId();
    std::string title = content->getTitle();

    Query* readQuery = new Query(Query::SELECT, database, table);
    readQuery->addParameter("id", nullptr, "int");
    readQuery->addParameter("movie_title", nullptr, "string");
    if (id != nullptr) {readQuery->addWhereParameter("id", id, "int");}
    return readQuery;
}

Query* MySQLContentRepo::MySQLupdate(Content* content)
{
    int* id = content->getId();
    if (*id == -1) { return nullptr; }

    std::string title = content->getTitle();

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("movie_title", &id, "string");
    updateQuery->addWhereParameter("id", &title, "int");
    return updateQuery;
}

Query* MySQLContentRepo::MySQLremove(Content* content)
{
    int* id = content->getId();
    if (*id == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, database, table);
    removeQuery->addWhereParameter("id", &id, "int");
    return removeQuery;
}