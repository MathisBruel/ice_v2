#include <stdlib.h>
#include <iostream>
#include <memory>
#include <map>

#include "Cpl.h"
#include "Poco/Mutex.h"
#include "portable_timer.h"

#pragma once

class Script
{

public:

    enum ScriptType
    {
        FEATURE = 1,
        LOOP,
        SAS,
        UNKNOWN
    };

    Script();
    ~Script();

    void setId(int id) {this->id = id;}
    void setDatas(std::string name, std::string cis_name, std::string lvi_name, std::string sha1Lvi);
    void setLink(ScriptType type, std::string path, std::string version);

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* getQuery(int* id = nullptr);

    static std::map<int, std::shared_ptr<Script>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<Script> loadFromResult(ResultQuery* result);

    std::string toXmlString();

    // -- getters
    int getId() {return id;}
    std::string getName() {return name;}
    std::string getPath() {return path;}
    ScriptType getType() {return type;}
    std::string getVersion() {return version;}


private:

    // -- general
    int id;
    std::string name;
    std::string path;
    std::string sha1Lvi;
    ScriptType type;
    std::string version;
    std::string cis_name;
    std::string lvi_name;

    static std::string database;
    static std::string table;
};