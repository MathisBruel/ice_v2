#include "Data/Server.h"

std::string Server::database = "ice";
std::string Server::table = "server";

Server::Server()
{
    id = -1;
    id_auditorium = -1;
    type = TypeServer::FEATURE;
    ip = "";
}
Server::~Server() {}

void Server::setDatas(int id_auditorium, TypeServer type, std::string ip)
{
    this->id_auditorium = id_auditorium;
    this->type = type;
    this->ip = ip;
}

Query* Server::createQuery()
{
    if (id != -1) {
        return nullptr;
    }

    Query* createQuery = new Query(Query::INSERT, database, table);
    createQuery->addParameter("id_auditorium", &id_auditorium, "int");
    createQuery->addParameter("type_server", &type, "int");
    createQuery->addParameter("ip", &ip, "string");
    return createQuery;
}
Query* Server::updateQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* updateQuery = new Query(Query::UPDATE, database, table);
    updateQuery->addParameter("id_auditorium", &id_auditorium, "int");
    updateQuery->addParameter("type_server", &type, "int");
    updateQuery->addParameter("ip", &ip, "string");
    updateQuery->addWhereParameter("id", &id, "int");
    return updateQuery;
}
Query* Server::deleteQuery()
{
    if (id == -1) {
        return nullptr;
    }

    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id", &id, "int");
    return deleteQuery;
}
Query* Server::deleteQueryForAuditorium(int* id_auditorium)
{
    Query* deleteQuery = new Query(Query::REMOVE, database, table);
    deleteQuery->addWhereParameter("id_auditorium", id_auditorium, "int");
    return deleteQuery;
}

Query* Server::getQuery(int* id)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_auditorium", nullptr, "int");
    getQuery->addParameter("type_server", nullptr, "int");
    getQuery->addParameter("ip", nullptr, "string");
    if (id != nullptr) {getQuery->addWhereParameter("id", id, "int");}
    return getQuery;
}
Query* Server::getQueryForAuditorium(int* id_auditorium)
{
    Query* getQuery = new Query(Query::SELECT, database, table);
    getQuery->addParameter("id", nullptr, "int");
    getQuery->addParameter("id_auditorium", nullptr, "int");
    getQuery->addParameter("type_server", nullptr, "int");
    getQuery->addParameter("ip", nullptr, "string");
    getQuery->addWhereParameter("id_auditorium", id_auditorium, "int");
    return getQuery;
}

std::map<int, std::shared_ptr<Server>> Server::loadListFromResult(ResultQuery* result)
{
    std::map<int, std::shared_ptr<Server>> list;
    for (int i = 0; i < result->getNbRows(); i++) {
        std::shared_ptr<Server> server = std::make_shared<Server>(Server());
        server->id = *result->getIntValue(i, "id");
        server->id_auditorium = *result->getIntValue(i, "id_auditorium");
        server->type = (Server::TypeServer)(*result->getIntValue(i, "type_server"));
        server->ip = *result->getStringValue(i, "ip");
        list.insert_or_assign(server->id, server);
    }

    return list;
}
std::shared_ptr<Server> Server::loadFromResult(ResultQuery* result)
{
    if (result->getNbRows() == 0) {
        return nullptr;
    }
    std::shared_ptr<Server> server = std::make_shared<Server>(Server());
    server->id = *result->getIntValue(0, "id");
    server->id_auditorium = *result->getIntValue(0, "id_auditorium");
    server->type = (Server::TypeServer)(*result->getIntValue(0, "type_server"));
    server->ip = *result->getStringValue(0, "ip");
    return server;
}

std::string Server::toXmlString()
{
    std::string xml = "<server";

    // -- general params
    xml += " id=\"" + std::to_string(id) + "\"";
    xml += " id_auditorium=\"" + std::to_string(id_auditorium) + "\"";
    xml += " type_server=\"" + std::to_string(type) + "\"";
    xml += " ip=\"" + ip + "\"";
    xml += " />";

    return xml;
}