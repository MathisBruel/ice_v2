#include <stdlib.h>
#include <iostream>
#include <map>
#include <memory>

#include "Server.h"
#include "Poco/Mutex.h"
#include "portable_timer.h"

#pragma once

class Auditorium
{

public:

    enum TypeIms
    {
        DOLBY = 1,
        BARCO,
        CHRISTIE
    };

    Auditorium();
    ~Auditorium();

    void setId(int id) {this->id = id;}
    void setDatas(int id_cinema, std::string name, int room);
    void setIms(TypeIms type_ims, std::string ip_ims, int port_ims, std::string user_ims, std::string pass_ims);

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* deleteQueryForCinema(int* id_cinema);
    static Query* getQuery(int* id = nullptr);
    static Query* getQueryForCinema(int* id_cinema);

    static std::map<int, std::shared_ptr<Auditorium>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<Auditorium> loadFromResult(ResultQuery* result);

    void setFeatureServer(std::shared_ptr<Server> server) {featureServer = server;}
    void setSasServer(std::shared_ptr<Server> server) {sasServer = server;}

    std::string toXmlString(bool printChild);

    // -- general getters
    int getId() {return id;}
    int getIdCinema() {return id_cinema;}
    std::string getName() {return name;}
    int getRoomNumber() {return  room;}
    TypeIms getTypeIms() {return type_ims;}
    std::string getIp() {return ip_ims;}
    int getPort() {return port_ims;}
    std::string getUser() {return user_ims;}
    std::string getPass() {return pass_ims;}

    // -- execution getters
    std::shared_ptr<Server> getFeatureServer() {return featureServer;}
    std::shared_ptr<Server> getSasServer() {return sasServer;}

private:

    // -- general
    int id;
    int id_cinema;
    std::string name;
    int room;
    TypeIms type_ims;
    std::string ip_ims;
    int port_ims;
    std::string user_ims;
    std::string pass_ims;

    // -- at execution
    std::shared_ptr<Server> featureServer;
    std::shared_ptr<Server> sasServer;

    static std::string database;
    static std::string table;
};