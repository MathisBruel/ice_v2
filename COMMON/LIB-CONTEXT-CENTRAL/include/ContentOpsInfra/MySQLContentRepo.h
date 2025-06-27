#pragma once
#include <iostream>
#include "Query.h"

#include "ContentOpsDomain/COD_ContentRepo.h"

class COD_Content;  

class MySQLContentRepo : public COD_ContentRepo
{
public:
    void Create(COD_Content* content) override { _query = MySQLcreate(content); }
    void Read(COD_Content* content) override { _query = MySQLread(content); }
    void Update(COD_Content* content) override { _query = MySQLupdate(content); }
    void Remove(COD_Content* content) override { _query = MySQLremove(content); }

    Query* MySQLcreate(COD_Content* content);
    Query* MySQLread(COD_Content* content);
    Query* MySQLupdate(COD_Content* content);
    Query* MySQLremove(COD_Content* content);
    
    Query* GetQuery() { return _query; }

private:
    static std::string _database;
    static std::string _table;

    int* _id;
    std::string _title;
    Query* _query;
};