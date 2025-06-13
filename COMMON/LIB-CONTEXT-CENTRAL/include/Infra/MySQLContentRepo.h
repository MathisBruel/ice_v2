#pragma once
#include <iostream>
#include "Query.h"

#include "Domain/ContentRepo.h"

#include "Infra/MySQLDBConnection.h"

class Content;  

class MySQLContentRepo : public ContentRepo
{
public:
    void Create(Content* content) override { _query = MySQLcreate(content); }
    void Read(Content* content) override { _query = MySQLread(content); }
    void Update(Content* content) override { _query = MySQLupdate(content); }
    void Remove(Content* content) override { _query = MySQLremove(content); }

    Query* MySQLcreate(Content* content);
    Query* MySQLread(Content* content);
    Query* MySQLupdate(Content* content);
    Query* MySQLremove(Content* content);
    
    Query* GetQuery() { return _query; }

private:
    static std::string _database;
    static std::string _table;

    int* _id;
    std::string _title;
    Query* _query;
    MySQLDBConnection * _dbConnection;
};