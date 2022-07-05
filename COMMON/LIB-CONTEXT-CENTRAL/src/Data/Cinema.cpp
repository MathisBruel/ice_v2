#include "Data/Cinema.h"

std::string Cinema::database = "ice";
std::string Cinema::table = "cinema";

Cinema::Cinema() 
{
    id = -1;
    name = "";
}

Cinema::~Cinema() {}

void Cinema::addAuditorium(std::shared_ptr<Auditorium> auditorium)
{
    auditoriums.push_back(auditorium);
}

Query* Cinema::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("name", &name, "string");
    return createQuery;
}
Query* Cinema::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("name", &name, "string");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* Cinema::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* Cinema::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("name", nullptr, "string");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}

std::map<int, std::shared_ptr<Cinema>> Cinema::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<Cinema>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<Cinema> cinema = std::make_shared<Cinema>(Cinema());
        cinema->id = *result->getIntValue(i, "id");
        cinema->name = *result->getStringValue(i, "name");
        list.insert_or_assign(cinema->id, cinema);
    }

    return list;
}
std::shared_ptr<Cinema> Cinema::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<Cinema> cinema = std::make_shared<Cinema>(Cinema());
    cinema->id = *result->getIntValue(0, "id");
    cinema->name = *result->getStringValue(0, "name");
    return cinema;
}

std::string Cinema::toXmlString(bool printChild)
{
    std::string xml = "<cinema";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " name=\"" + name + "\"";

    if (printChild) {
        xml += ">";

        // -- lib contents
        xml += "<auditoriums>";
        for (std::shared_ptr<Auditorium> audi : auditoriums) {
            xml += audi->toXmlString(false);
        }
        xml += "</auditoriums>";

        // -- finish
        xml += "</cinema>";
    }
    else {
        xml += " />";
    }

    return xml;
}