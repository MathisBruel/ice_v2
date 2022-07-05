#include "Data/Feature.h"

std::string Feature::database = "ice";
std::string Feature::table = "feature";

Feature::Feature()
{
    id = -1;
    name = "";
}

Feature::~Feature() {}

void Feature::setDatas(std::string name) 
{
    this->name = name;
}
void Feature::addRelease(std::shared_ptr<Release> release)
{
    releases.push_back(release);
}

Query* Feature::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("name", &name, "string");
    return createQuery;
}
Query* Feature::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("name", &name, "string");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* Feature::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* Feature::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("name", nullptr, "string");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}

std::map<int, std::shared_ptr<Feature>> Feature::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<Feature>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<Feature> feature = std::make_shared<Feature>(Feature());
        feature->id = *result->getIntValue(i, "id");
        feature->name = *result->getStringValue(i, "name");
        list.insert_or_assign(feature->id, feature);
    }

    return list;
}

std::shared_ptr<Feature> Feature::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<Feature> feature = std::make_shared<Feature>(Feature());
    feature->id = *result->getIntValue(0, "id");
    feature->name = *result->getStringValue(0, "name");
    return feature;
}

std::string Feature::toXmlString(bool printChild)
{
    std::string xml = "<feature";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " name=\"" + name + "\"";

    if (printChild) {
        xml += ">";

        xml += "<releases>";
        for (std::shared_ptr<Release> release : releases) {
            xml += release->toXmlString(false);
        }
        xml += "</releases>";

        // -- finish
        xml += "</feature>";
    }
    else {
        xml += " />";
    }

    return xml;
}