#include "Data/Script.h"

std::string Script::database = "ice";
std::string Script::table = "script";

Script::Script()
{
    id = -1;
    name = "";
    path = "";
    cis_name = "";
    lvi_name = "";
    type = ScriptType::UNKNOWN;
    version = "";
}
Script::~Script() {}

void Script::setDatas(std::string name, std::string cis_name, std::string lvi_name)
{
    this->name = name;
    this->cis_name = cis_name;
    this->lvi_name = lvi_name;
}
void Script::setLink(ScriptType type, std::string path, std::string version)
{
    this->type = type;
    this->path = path;
    this->version = version;
}

Query* Script::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("name", &name, "string");
    createQuery->addParameter("cis_name", &cis_name, "string");
    createQuery->addParameter("lvi_name", &lvi_name, "string");
    createQuery->addParameter("path", &path, "string");
    createQuery->addParameter("type", &type, "int");
    createQuery->addParameter("version", &version, "string");
    return createQuery;
}
Query* Script::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("name", &name, "string");
    updateQuery->addParameter("cis_name", &cis_name, "string");
    updateQuery->addParameter("lvi_name", &lvi_name, "string");
    updateQuery->addParameter("path", &path, "string");
    updateQuery->addParameter("type", &type, "int");
    updateQuery->addParameter("version", &version, "string");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* Script::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* Script::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("name", nullptr, "string");
    getQuery->addParameter("cis_name", nullptr, "string");
    getQuery->addParameter("lvi_name", nullptr, "string");
    getQuery->addParameter("path", nullptr, "string");
    getQuery->addParameter("type", nullptr, "int");
    getQuery->addParameter("version", nullptr, "string");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}

std::map<int, std::shared_ptr<Script>> Script::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<Script>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<Script> script = std::make_shared<Script>(Script());
        script->id = *result->getIntValue(i, "id");
        script->name = *result->getStringValue(i, "name");
        script->cis_name = *result->getStringValue(i, "cis_name");
        script->lvi_name = *result->getStringValue(i, "lvi_name");
        script->path = *result->getStringValue(i, "path");
        script->type = (Script::ScriptType)(*result->getIntValue(i, "type"));
        script->version = *result->getStringValue(i, "version");
        list.insert_or_assign(script->id, script);
    }

    return list;
}
std::shared_ptr<Script> Script::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<Script> script = std::make_shared<Script>(Script());
    script->id = *result->getIntValue(0, "id");
    script->name = *result->getStringValue(0, "name");
    script->cis_name = *result->getStringValue(0, "cis_name");
    script->lvi_name = *result->getStringValue(0, "lvi_name");
    script->path = *result->getStringValue(0, "path");
    script->type = (Script::ScriptType)(*result->getIntValue(0, "type"));
    script->version = *result->getStringValue(0, "version");
    return script;
}

std::string Script::toXmlString()
{
    std::string xml = "<script";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " name=\"" + name + "\"";
    xml += " cis_name=\"" + cis_name + "\"";
    xml += " lvi_name=\"" + lvi_name + "\"";
    xml += " path=\"" + path + "\"";
    xml += " type=\"" + std::to_string(type) + "\"";
    xml += " version=\"" + version + "\"";
    xml += " />";

    return xml;
}