#include <stdlib.h>
#include <iostream>

#pragma once

#include "Data/Cinema.h"
#include "Data/Release.h"

class Group
{

public:

    Group();
    ~Group();

    void setId(int id) {this->id = id;}
    void setDatas(std::string name, std::string description) {this->name = name; this->description = description;}
    void addCinema(std::shared_ptr<Cinema> cinema) {cinemas.push_back(cinema);}
    void addRelease(std::shared_ptr<Release> release) {releases.push_back(release);}

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* getQuery(int* id = nullptr);

    static std::map<int, std::shared_ptr<Group>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<Group> loadFromResult(ResultQuery* result);

    std::string toXmlString(bool printChild);

    int getId() {return id;}
    std::string getName() {return name;}
    std::string getDescription() {return description;}
    std::vector<std::shared_ptr<Cinema>> getCinemas() {return cinemas;}
    std::vector<std::shared_ptr<Release>> getReleases() {return releases;}

private:

    int id;
    std::string name; 
    std::string description;

    // -- at execution
    std::vector<std::shared_ptr<Cinema>> cinemas;
    std::vector<std::shared_ptr<Release>> releases;

    static std::string database;
    static std::string table;
};