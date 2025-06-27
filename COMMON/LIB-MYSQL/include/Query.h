#include <stdlib.h>
#include <iostream>
#include <map>

#include "Poco/Logger.h"
#include "Poco/DateTime.h"

#pragma once

class Query
{

public:

    enum TypeQuery
    {
        SELECT,
        INSERT,
        UPDATE,
        REMOVE,
        CUSTOM
    };

    Query(TypeQuery type, std::string database, std::string table);
    ~Query();

    void addParameter(std::string name, void* value, std::string type);
    void addWhereParameter(std::string name, void* value, std::string type);
    std::map<std::string, std::string> getSchema() {return schemaParameter;}

    std::string getQueryString();
    bool isSelectQuery() {return type == SELECT;}
    bool isInsertQuery() {return type == INSERT;}
    
    // Méthodes pour les requêtes personnalisées
    void setCustomSQL(const std::string& sql) { customSQL = sql; }
    std::string getCustomSQL() const { return customSQL; }

private:

    // -- base parameters
    TypeQuery type;
    std::string database;
    std::string table;
    std::string customSQL; // Pour les requêtes personnalisées

    // -- association key / type parameters (string, int, double, date)
    std::map<std::string, std::string> schemaParameter;
    
    // -- association key / value
    std::map<std::string, void*> valueParameter;

    // --  association key / type parameters (string, int, double, date)
    std::map<std::string, std::string> schemaWhereParameter;

    // -- association key / value
    std::map<std::string, void*> valueWhereParameter;
};