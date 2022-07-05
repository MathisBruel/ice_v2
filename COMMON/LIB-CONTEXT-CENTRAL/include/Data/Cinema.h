#include <stdlib.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "Auditorium.h"

#pragma once

class Cinema
{

public:

    Cinema();
    ~Cinema();

    void setId(int id) {this->id = id;}
    void setDatas(std::string name) {this->name = name;}
    void addAuditorium(std::shared_ptr<Auditorium> auditorium);

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* getQuery(int* id = nullptr);

    static std::map<int, std::shared_ptr<Cinema>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<Cinema> loadFromResult(ResultQuery* result);

    // -- getters
    int getId() {return id;}
    std::string getName() {return name;}
    std::vector<std::shared_ptr<Auditorium>> getAuditoriums() {return auditoriums;}
    
    std::string toXmlString(bool printChild);

private:

    int id;
    std::string name;

    // -- list of ICE auditorium
    std::vector<std::shared_ptr<Auditorium>> auditoriums;

    static std::string database;
    static std::string table;
};