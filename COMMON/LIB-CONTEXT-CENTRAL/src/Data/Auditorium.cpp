#include "Data/Auditorium.h"

std::string Auditorium::database = "ice";
std::string Auditorium::table = "auditorium";

Auditorium::Auditorium() 
{
    id = -1;
    id_cinema = -1;
    name = "";
    room = -1;
    type_ims = TypeIms::DOLBY;
    ip_ims = "";
    port_ims = -1;
    user_ims = "";
    pass_ims = "";
}

Auditorium::~Auditorium()
{

}

void Auditorium::setDatas(int id_cinema, std::string name, int room) 
{
    this->id_cinema = id_cinema;
    this->name = name; 
    this->room = room;
}
void Auditorium::setIms(TypeIms type_ims, std::string ip_ims, int port_ims, std::string user_ims, std::string pass_ims) 
{
    this->type_ims = type_ims;
    this->ip_ims = ip_ims;
    this->port_ims = port_ims;
    this->user_ims = user_ims;
    this->pass_ims = pass_ims;
}

Query* Auditorium::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("id_cinema", &id_cinema, "int");
    createQuery->addParameter("name", &name, "string");
    createQuery->addParameter("room", &room, "int");
    createQuery->addParameter("type_ims", &type_ims, "int");
    createQuery->addParameter("ip_ims", &ip_ims, "string");
    createQuery->addParameter("port_ims", &port_ims, "int");
    createQuery->addParameter("user_ims", &user_ims, "string");
    createQuery->addParameter("pass_ims", &pass_ims, "string");
    return createQuery;
}
Query* Auditorium::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("id_cinema", &id_cinema, "int");
    updateQuery->addParameter("name", &name, "string");
    updateQuery->addParameter("room", &room, "int");
    updateQuery->addParameter("type_ims", &type_ims, "int");
    updateQuery->addParameter("ip_ims", &ip_ims, "string");
    updateQuery->addParameter("port_ims", &port_ims, "int");
    updateQuery->addParameter("user_ims", &user_ims, "string");
    updateQuery->addParameter("pass_ims", &pass_ims, "string");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* Auditorium::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* Auditorium::deleteQueryForCinema(int* id_cinema)
{
    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id_cinema", id_cinema, "int");
    return deleteQuery;
}
Query* Auditorium::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_cinema", nullptr, "int");
    getQuery->addParameter("name", nullptr, "string");
    getQuery->addParameter("room", nullptr, "int");
    getQuery->addParameter("type_ims", nullptr, "int");
    getQuery->addParameter("ip_ims", nullptr, "string");
    getQuery->addParameter("port_ims", nullptr, "int");
    getQuery->addParameter("user_ims", nullptr, "string");
    getQuery->addParameter("pass_ims", nullptr, "string");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}
Query* Auditorium::getQueryForCinema(int* id_cinema)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_cinema", nullptr, "int");
    getQuery->addParameter("name", nullptr, "string");
    getQuery->addParameter("room", nullptr, "int");
    getQuery->addParameter("type_ims", nullptr, "int");
    getQuery->addParameter("ip_ims", nullptr, "string");
    getQuery->addParameter("port_ims", nullptr, "int");
    getQuery->addParameter("user_ims", nullptr, "string");
    getQuery->addParameter("pass_ims", nullptr, "string");
    getQuery->addWhereParameter("id_cinema", id_cinema, "int");
    return getQuery;
}

std::map<int, std::shared_ptr<Auditorium>> Auditorium::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<Auditorium>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<Auditorium> audi = std::make_shared<Auditorium>(Auditorium());
        audi->id = *result->getIntValue(i, "id");
        audi->id_cinema = *result->getIntValue(i, "id_cinema");
        audi->name = *result->getStringValue(i, "name");
        audi->room = *result->getIntValue(i, "room");
        audi->type_ims = (Auditorium::TypeIms)(*result->getIntValue(i, "type_ims"));
        audi->ip_ims = *result->getStringValue(i, "ip_ims");
        audi->port_ims = *result->getIntValue(i, "port_ims");
        audi->user_ims = *result->getStringValue(i, "user_ims");
        audi->pass_ims = *result->getStringValue(i, "pass_ims");

        list.insert_or_assign(audi->id, audi);
    }

    return list;
}
std::shared_ptr<Auditorium> Auditorium::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<Auditorium> audi = std::make_shared<Auditorium>(Auditorium());
    audi->id = *result->getIntValue(0, "id");
    audi->id_cinema = *result->getIntValue(0, "id_cinema");
    audi->name = *result->getStringValue(0, "name");
    audi->room = *result->getIntValue(0, "room");
    audi->type_ims = (Auditorium::TypeIms)(*result->getIntValue(0, "type_ims"));
    audi->ip_ims = *result->getStringValue(0, "ip_ims");
    audi->port_ims = *result->getIntValue(0, "port_ims");
    audi->user_ims = *result->getStringValue(0, "user_ims");
    audi->pass_ims = *result->getStringValue(0, "pass_ims");
    return audi;
}

std::string Auditorium::toXmlString(bool printChild)
{
    std::string xml = "<auditorium";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " id_cinema=\"" + std::to_string(id_cinema) + "\"";
    xml += " name=\"" + name + "\"";
    xml += " room=\"" + std::to_string(room) + "\"";
    xml += " type_ims=\"" + std::to_string(type_ims) + "\"";
    xml += " ip_ims=\"" + ip_ims + "\"";
    xml += " port_ims=\"" + std::to_string(port_ims) + "\"";
    xml += " user_ims=\"" + user_ims + "\"";
    xml += " pass_ims=\"" + pass_ims + "\"";

    if (printChild) {
        xml += ">";
        
        if (featureServer != nullptr) {
            xml += featureServer->toXmlString();
        }
        if (sasServer != nullptr) {
            xml += sasServer->toXmlString();
        }

        // -- finish
        xml += "</auditorium>";
    }
    else {
        xml += " />";
    }

    return xml;
}