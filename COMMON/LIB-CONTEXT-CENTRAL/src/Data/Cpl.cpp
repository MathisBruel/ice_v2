#include "Data/Cpl.h"

std::string Cpl::database = "ice";
std::string Cpl::table = "cpl";

Cpl::Cpl() 
{
    id = -1;
    uuid = "";
    name = "";
    pathCpl = "";
    pathSync = "";
    sha1Sync = "";
}
Cpl::~Cpl() {}

void Cpl::setDatas(std::string uuid, std::string name)
{
    this->uuid = uuid;
    this->name = name;
}
void Cpl::setCplInfos(std::string pathCpl, std::string pathSync, std::string sha1Sync)
{
    this->pathCpl = pathCpl;
    this->pathSync = pathSync;
    this->sha1Sync = sha1Sync;
}

Query* Cpl::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("name", &name, "string");
    createQuery->addParameter("uuid", &uuid, "string");
    createQuery->addParameter("path_cpl", &pathCpl, "string");
    createQuery->addParameter("path_sync", &pathSync, "string");
    createQuery->addParameter("sha1_sync", &sha1Sync, "string");
    return createQuery;
}
Query* Cpl::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("name", &name, "string");
    updateQuery->addParameter("uuid", &uuid, "string");
    updateQuery->addParameter("path_cpl", &pathCpl, "string");
    updateQuery->addParameter("path_sync", &pathSync, "string");
    updateQuery->addParameter("sha1_sync", &sha1Sync, "string");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* Cpl::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* Cpl::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("name", nullptr, "string");
    getQuery->addParameter("uuid", nullptr, "string");
    getQuery->addParameter("path_cpl", nullptr, "string");
    getQuery->addParameter("path_sync", nullptr, "string");
    getQuery->addParameter("sha1_sync", nullptr, "string");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}

std::map<int, std::shared_ptr<Cpl>> Cpl::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<Cpl>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<Cpl> cpl = std::make_shared<Cpl>(Cpl());
        cpl->id = *result->getIntValue(i, "id");
        cpl->name = *result->getStringValue(i, "name");
        cpl->uuid = *result->getStringValue(i, "uuid");
        cpl->pathCpl = *result->getStringValue(i, "path_cpl");
        cpl->pathSync = *result->getStringValue(i, "path_sync");
        cpl->sha1Sync = *result->getStringValue(i, "sha1_sync");
        list.insert_or_assign(cpl->id, cpl);
    }
    return list;
}
std::shared_ptr<Cpl> Cpl::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<Cpl> cpl = std::make_shared<Cpl>(Cpl());
    cpl->id = *result->getIntValue(0, "id");
    cpl->name = *result->getStringValue(0, "name");
    cpl->uuid = *result->getStringValue(0, "uuid");
    cpl->pathCpl = *result->getStringValue(0, "path_cpl");
    cpl->pathSync = *result->getStringValue(0, "path_sync");
    cpl->sha1Sync = *result->getStringValue(0, "sha1_sync");
    return cpl;
}

std::string Cpl::toXmlString()
{
    std::string xml = "<cpl";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " name=\"" + name + "\"";
    xml += " uuid=\"" + uuid + "\"";
    xml += " pathCpl=\"" + pathCpl + "\"";
    xml += " pathSync=\"" + pathSync + "\"";
    xml += " sha1Sync=\"" + sha1Sync + "\"";
    xml += " />";

    return xml;
}