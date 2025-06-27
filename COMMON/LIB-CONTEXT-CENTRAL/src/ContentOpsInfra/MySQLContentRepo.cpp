#include "ContentOpsInfra/MySQLContentRepo.h"

#include "ContentOpsDomain/COD_Content.h"

std::string MySQLContentRepo::_database = "ice";
std::string MySQLContentRepo::_table = "content";

MySQLContentRepo::MySQLContentRepo()
{
    _query = nullptr;
}

MySQLContentRepo::~MySQLContentRepo()
{
}

std::unique_ptr<Query> MySQLContentRepo::MySQLcreate(COD_Content* content)
{
    _contentIds = content->GetContentIdPtr();
    if (*_contentIds != -1) { return nullptr; }
    _contentTitles = *content->GetContentTitlePtr();
    
    std::unique_ptr<Query> createQuery = std::make_unique<Query>(Query::INSERT, _database, _table);
    createQuery->addParameter("title", &_contentTitles, "string");
    return createQuery;
}

std::unique_ptr<Query> MySQLContentRepo::MySQLread(COD_Content* content)
{
    _contentIds = content->GetContentIdPtr();
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("title", nullptr, "string");
    if (*_contentIds != -1) {readQuery->addWhereParameter("id_content", _contentIds, "int");}
    return readQuery;
}

std::unique_ptr<Query> MySQLContentRepo::MySQLread()
{
    std::unique_ptr<Query> readQuery = std::make_unique<Query>(Query::SELECT, _database, _table);
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("title", nullptr, "string");
    return readQuery;
}

std::unique_ptr<Query> MySQLContentRepo::MySQLupdate(COD_Content* content)
{
    _contentIds = content->GetContentIdPtr();
    if (*_contentIds == -1) { return nullptr; }

    _contentTitles = *content->GetContentTitlePtr();

    std::unique_ptr<Query> updateQuery = std::make_unique<Query>(Query::UPDATE, _database, _table);
    updateQuery->addParameter("title", &_contentTitles, "string");
    updateQuery->addWhereParameter("id_content", _contentIds, "int");
    return updateQuery;
}

std::unique_ptr<Query> MySQLContentRepo::MySQLremove(COD_Content* content)
{
    _contentIds = content->GetContentIdPtr();
    if (*_contentIds == -1) { return nullptr; }

    std::unique_ptr<Query> removeQuery = std::make_unique<Query>(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_content", _contentIds, "int");
    return removeQuery;
}

void MySQLContentRepo::Create(COD_Content* content) 
{
    _query = MySQLcreate(content);
}

void MySQLContentRepo::Read(COD_Content* content) 
{
    _query = MySQLread(content);
}

void MySQLContentRepo::Update(COD_Content* content) 
{
    _query = MySQLupdate(content);
}

void MySQLContentRepo::Remove(COD_Content* content) 
{
    _query = MySQLremove(content);
}

std::unique_ptr<ResultQuery> MySQLContentRepo::getContents()
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::unique_ptr<Query> query = MySQLread();
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}

std::unique_ptr<ResultQuery> MySQLContentRepo::getContent(int contentId)
{
    std::unique_ptr<MySQLDBConnection> dbConn = std::make_unique<MySQLDBConnection>();
    dbConn->InitConnection();
    std::unique_ptr<Query> query = std::make_unique<Query>(Query::SELECT, _database, _table);
    query->addParameter("id_content", nullptr, "int");
    query->addParameter("title", nullptr, "string");
    query->addWhereParameter("id_content", &contentId, "int");
    std::unique_ptr<ResultQuery> result(dbConn->ExecuteQuery(query.get()));
    return result;
}