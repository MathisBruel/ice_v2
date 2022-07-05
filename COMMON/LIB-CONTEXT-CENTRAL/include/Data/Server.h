#include <stdlib.h>
#include <iostream>
#include <vector>

#include "Script.h"
#include "Poco/Mutex.h"
#include "portable_timer.h"
#include "Query.h"
#include "ResultQuery.h"

#pragma once

class Server
{

public:

    enum TypeServer
    {
        FEATURE = 1,
        SAS
    };

    Server();
    ~Server();

    void setId(int id) {this->id = id;}
    void setDatas(int id_auditorium, TypeServer type, std::string ip);

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* deleteQueryForAuditorium(int* id_auditorium);
    static Query* getQuery(int* id = nullptr);
    static Query* getQueryForAuditorium(int* id_auditorium);

    static std::map<int, std::shared_ptr<Server>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<Server> loadFromResult(ResultQuery* result);

    std::string toXmlString();

    // -- get generals
    int getId() {return id;}
    int getIdAuditorium() {return id_auditorium;}
    TypeServer getTypeServer() {return type;}
    std::string getIp() {return ip;}

private:

    // -- generals
    int id;
    int id_auditorium;
    TypeServer type;
    std::string ip;

    static std::string database;
    static std::string table;
};