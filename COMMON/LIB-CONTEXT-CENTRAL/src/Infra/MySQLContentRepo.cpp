#include "Infra/MySQLContentRepo.h"

#include "CentralContext.h"
#include "Domain/Content.h"

std::string MySQLContentRepo::_database = "ice";
std::string MySQLContentRepo::_table = "content";

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
    CentralContext* context = CentralContext::getCurrentContext();
    this->_dbConnection = new MySQLDBConnection(context->getDatabaseConnector());
    content->SetStateMachine(content->CreateStateMachine(*_id, this->_dbConnection));
    Query* readQuery = new Query(Query::SELECT, _database, _table);
    readQuery->addParameter("id_content", nullptr, "int");
    readQuery->addParameter("title", nullptr, "string");
    if (*_id != -1) {readQuery->addWhereParameter("id_content", &_id, "int");}
    return readQuery;
}

Query* MySQLContentRepo::MySQLupdate(Content* content)
{
    _id = content->GetContentId();
    if (*_id == -1) { return nullptr; }

    _title = content->GetContentTitle();

    Query* updateQuery = new Query(Query::UPDATE, _database, _table);
    updateQuery->addParameter("title", &_id, "string");
    updateQuery->addWhereParameter("id_content", &_title, "int");
    return updateQuery;
}

Query* MySQLContentRepo::MySQLremove(Content* content)
{
    _id = content->GetContentId();
    if (*_id == -1) { return nullptr; }

    Query* removeQuery = new Query(Query::REMOVE, _database, _table);
    removeQuery->addWhereParameter("id_content", &_id, "int");
    return removeQuery;
}