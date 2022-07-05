#include <stdlib.h>
#include <iostream>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>

#include "Converter.h"
#include "Query.h"
#include "ResultQuery.h"

#pragma once

class Cpl
{

public:

    Cpl();
    ~Cpl();

    void setId(int id) {this->id = id;}
    void setDatas(std::string uuid, std::string name);
    void setCplInfos(std::string pathCpl, std::string pathSync, std::string sha1Sync);

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* getQuery(int* id = nullptr);

    static std::map<int, std::shared_ptr<Cpl>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<Cpl> loadFromResult(ResultQuery* result);

    std::string toXmlString();

    // -- getters
    int getId() {return id;}
    std::string getUuid() {return uuid;}
    std::string getName() {return name;}
    std::string getSha1Sync() {return sha1Sync;}
    std::string getCplPath() {return pathCpl;}
    std::string getSyncPath() {return pathSync;}

private:

    // -- general
    int id;
    std::string uuid;
    std::string name;
    std::string sha1Sync;
    // -- specific for synchro
    std::string pathCpl;
    std::string pathSync;

    static std::string database;
    static std::string table;
};