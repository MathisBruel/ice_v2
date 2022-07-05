#include "Data/Group.h"

std::string Group::database = "ice";
std::string Group::table = "group";

Group::Group()
{
    id = -1;
    name = ""; 
    description = "";
}
Group::~Group() {}

Query* Group::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("name", &name, "string");
    createQuery->addParameter("description", &description, "string");
    return createQuery;
}
Query* Group::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("name", &name, "string");
    updateQuery->addParameter("description", &description, "string");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* Group::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* Group::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("name", nullptr, "string");
    getQuery->addParameter("description", nullptr, "string");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}

std::map<int, std::shared_ptr<Group>> Group::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<Group>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<Group> group = std::make_shared<Group>(Group());
        group->id = *result->getIntValue(i, "id");
        group->name = *result->getStringValue(i, "name");
        group->description = *result->getStringValue(i, "description");
        list.insert_or_assign(group->id, group);
    }

    return list;
}
std::shared_ptr<Group> Group::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<Group> group = std::make_shared<Group>(Group());
    group->id = *result->getIntValue(0, "id");
    group->name = *result->getStringValue(0, "name");
    group->description = *result->getStringValue(0, "description");
    return group;
}

std::string Group::toXmlString(bool printChild)
{
    std::string xml = "<group";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " name=\"" + name + "\"";
    xml += " description=\"" + description + "\"";

    if (printChild) {
        xml += ">";

        // -- cinemas
        xml += "<cinemas>";
        for (std::shared_ptr<Cinema> cinema : cinemas) {
            xml += cinema->toXmlString(false);
        }
        xml += "</cinemas>";

        // -- templates
        xml += "<releases>";
        for (std::shared_ptr<Release> release : releases) {
            xml += release->toXmlString(false);
        }
        xml += "</releases>";

        // -- finish
        xml += "</group>";
    }
    else {
        xml += " />";
    }

    return xml;
}