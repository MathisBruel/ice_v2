#include "Data/ReleaseCut.h"

std::string ReleaseCut::database = "ice";
std::string ReleaseCut::table = "release_cut";

ReleaseCut::ReleaseCut()
{
    id = -1;
    id_release = -1;
    description = "";
    position = -1;
    size = -1;
}
ReleaseCut::~ReleaseCut() {}

Query* ReleaseCut::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("id_release", &id_release, "int");
    createQuery->addParameter("description", &description, "string");
    createQuery->addParameter("position", &position, "int");
    createQuery->addParameter("size", &size, "int");
    return createQuery;
}
Query* ReleaseCut::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("id_release", &id_release, "int");
    updateQuery->addParameter("description", &description, "string");
    updateQuery->addParameter("position", &position, "int");
    updateQuery->addParameter("size", &size, "int");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* ReleaseCut::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* ReleaseCut::deleteQueryForRelease(int* id_release)
{
    if (id_release == nullptr) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id_release", id_release, "int");
    return deleteQuery;
}
Query* ReleaseCut::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_release", nullptr, "int");
    getQuery->addParameter("description", nullptr, "string");
    getQuery->addParameter("position", nullptr, "int");
    getQuery->addParameter("size", nullptr, "int");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}
Query* ReleaseCut::getQueryForRelease(int* id_release)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_release", nullptr, "int");
    getQuery->addParameter("description", nullptr, "string");
    getQuery->addParameter("position", nullptr, "int");
    getQuery->addParameter("size", nullptr, "int");
    getQuery->addWhereParameter("id_release", id_release, "int");
    return getQuery;
}

std::map<int, std::shared_ptr<ReleaseCut>> ReleaseCut::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<ReleaseCut>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<ReleaseCut> cut = std::make_shared<ReleaseCut>(ReleaseCut());
        cut->id = *result->getIntValue(i, "id");
        cut->id_release = *result->getIntValue(i, "id_release");
        cut->description = *result->getStringValue(i, "description");
        cut->position = *result->getIntValue(i, "position");
        cut->size = *result->getIntValue(i, "size");
        list.insert_or_assign(cut->id, cut);
    }

    return list;
}
std::shared_ptr<ReleaseCut> ReleaseCut::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<ReleaseCut> cut = std::make_shared<ReleaseCut>(ReleaseCut());
    cut->id = *result->getIntValue(0, "id");
    cut->id_release = *result->getIntValue(0, "id_release");
    cut->description = *result->getStringValue(0, "description");
    cut->position = *result->getIntValue(0, "position");
    cut->size = *result->getIntValue(0, "size");
    return cut;
}

std::string ReleaseCut::toXmlString()
{
    std::string xml = "<releaseCut";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " id_release=\"" + std::to_string(id_release) + "\"";
    xml += " description=\"" + description + "\"";
    xml += " position=\"" + std::to_string(position) + "\"";
    xml += " size=\"" + std::to_string(size) + "\"";
    xml += " />";

    return xml;
}