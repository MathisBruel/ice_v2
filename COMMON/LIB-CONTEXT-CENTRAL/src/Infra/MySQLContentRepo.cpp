#include "Infra/MySQLContentRepo.h"

std::string MySQLContentRepo::_database = "ice";
std::string MySQLContentRepo::_table = "movie";

Query* MySQLContentRepo::MySQLcreate(Content* content)
{
    _id = content->GetContentId();
    if (*_id != -1) { return nullptr; }
    _title = content->GetContentTitle();
    
    Query* createQuery = new Query(Query::INSERT, _database, _table);
    createQuery->addParameter("title", &_title, "string");
    return createQuery;
}

Query* MySQLContentRepo::MySQLread(Content* content)
{
    _id = content->GetContentId();

    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_movie", nullptr, "int");
    readQuery->addParameter("title", nullptr, "string");
    if (*_id != -1) {readQuery->addWhereParameter("id_movie", &_id, "int");}
    return readQuery;
}

Query* MySQLContentRepo::MySQLupdate(Content* content)
{
    _id = content->GetContentId();
    if (*_id == -1) { return nullptr; }

    _title = content->GetContentTitle();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("title", &_id, "string");
    updateQuery->addWhereParameter("id_movie", &_title, "int");
    return updateQuery;
}

Query* MySQLContentRepo::MySQLremove(Content* content)
{
    _id = content->GetContentId();
    if (*_id == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_movie", &_id, "int");
    return removeQuery;
}