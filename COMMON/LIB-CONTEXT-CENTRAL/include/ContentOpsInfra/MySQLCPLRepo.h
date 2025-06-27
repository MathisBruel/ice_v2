#pragma once
#include <iostream>

#include "ContentOpsDomain/COD_CPLRepo.h"
#include "Query.h"

class MySQLCPLRepo : public COD_CPLRepo
{
public:
    void Create(COD_CPLRelease* cpl) override { _query = MySQLcreate(cpl); }
    void Read(COD_CPLRelease* cpl) override { _query = MySQLread(cpl); }
    void Update(COD_CPLRelease* cpl) override { _query = MySQLupdate(cpl); }
    void Remove(COD_CPLRelease* cpl) override { _query = MySQLremove(cpl); }

    Query* MySQLcreate(COD_CPLRelease* cpl);
    Query* MySQLread(COD_CPLRelease* cpl);
    Query* MySQLupdate(COD_CPLRelease* cpl);
    Query* MySQLremove(COD_CPLRelease* cpl);

    Query* GetQuery() { return _query; }

private:
    static std::string _database;
    static std::string _table;

    int* _cplIds;
    std::string _uuid;
    std::string _name;
    std::string _cplPath;
    
    Query* _query;
};