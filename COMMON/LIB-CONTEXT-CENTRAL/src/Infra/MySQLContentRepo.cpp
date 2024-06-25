#include "MySQLContentRepo.h"

std::string MySQLContentRepo::database = "ice";
std::string MySQLContentRepo::table = "movie";

Query* MySQLContentRepo::create(Content* content)
{
    if (content->getId() != -1) { return nullptr; }
    
    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("movie_title", &content->getTitle(), "string");
    return createQuery;
}

Query* MySQLContentRepo::read(Content* content)
{
    Query* readQuery = new Query(Query::SELECT, database, table);
    readQuery->addParameter("id", &content->getId(), "int");
    readQuery->addParameter("movie_title", &content->getTitle(), "string");
    if (content->id != nullptr) {readQuery->addWhereParameter("id", &content->getId(), "int");}
    return readQuery;
}

Query* MySQLContentRepo::update(Content* content)
{
    if (content->getId() == -1) { return nullptr; }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("movie_title", &content->getTitle(), "string");
    updateQuery->addWhereParameter("id", &content->getId(), "int");
    return updateQuery;
}

Query* MySQLContentRepo::remove(Content* content)
{
    if (content->getId() == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, database, table);
    removeQuery->addWhereParameter("id", &content->getId(), "int");
    return removeQuery;
}