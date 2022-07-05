#include <stdlib.h>
#include <iostream>
#include <map>

#include "Release.h"

#pragma once

class Feature
{

public:

    Feature();
    ~Feature();
    
    void setId(int id) {this->id = id;}
    void setDatas(std::string name);
    void addRelease(std::shared_ptr<Release> release);

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* getQuery(int* id = nullptr);

    static std::map<int, std::shared_ptr<Feature>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<Feature> loadFromResult(ResultQuery* result);

    std::string toXmlString(bool printChild);

    int getId() {return id;}
    std::string getName() {return name;}
    std::vector<std::shared_ptr<Release>> getReleases() {return releases;}

private:

    // -- general
    int id;
    std::string name;

    // -- releases
    std::vector<std::shared_ptr<Release>> releases;

    static std::string database;
    static std::string table;
};