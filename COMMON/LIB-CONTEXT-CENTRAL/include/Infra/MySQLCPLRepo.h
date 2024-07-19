#pragma once
#include <iostream>

#include "Domain/CPLRepo.h"
#include "Query.h"

class MySQLCPLRepo : public CPLRepo
{
public:
    void Create(CPLRelease* cpl) override { _query = MySQLcreate(cpl); }
    void Read(CPLRelease* cpl) override { _query = MySQLread(cpl); }
    void Update(CPLRelease* cpl) override { _query = MySQLupdate(cpl); }
    void Remove(CPLRelease* cpl) override { _query = MySQLremove(cpl); }

    Query* MySQLcreate(CPLRelease* cpl);
    Query* MySQLread(CPLRelease* cpl);
    Query* MySQLupdate(CPLRelease* cpl);
    Query* MySQLremove(CPLRelease* cpl);

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