#include <stdlib.h>
#include <iostream>
#include <map>
#include <memory>

#include "Query.h"
#include "ResultQuery.h"

#pragma once

class ReleaseCut
{

public:

    ReleaseCut();
    ~ReleaseCut();

    void setId(int id) {this->id = id;}
    void setDatas(std::string description, int position, int size) {this->description = description; this->position = position; this->size = size;}
    void setIdRelease(int id_release) {this->id_release = id_release;}

    Query* createQuery();
    Query* updateQuery();
    Query* deleteQuery();
    static Query* deleteQueryForRelease(int* id_release);
    static Query* getQuery(int* id = nullptr);
    static Query* getQueryForRelease(int* id_release);

    static std::map<int, std::shared_ptr<ReleaseCut>> loadListFromResult(ResultQuery* result);
    static std::shared_ptr<ReleaseCut> loadFromResult(ResultQuery* result);

    std::string toXmlString();

    // -- getters
    int getId() {return id;}
    int getIdRelease() {return id_release;}
    std::string getDescription() {return description;}
    int getPosition() {return position;}
    int getSize() {return size;}
    
private:

    // -- general
    int id;
    int id_release;
    std::string description;
    int position;
    int size;

    static std::string database;
    static std::string table;
};