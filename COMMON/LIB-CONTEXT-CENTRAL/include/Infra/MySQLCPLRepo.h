#pragma once
#include <iostream>

#include "Domain/CPLRepo.h"
#include "Query.h"

class MySQLCPLRepo : public CPLRepo
{
public:
    MySQLCPLRepo();
    ~MySQLCPLRepo();
    
    void Create(CPL* cpl) override { MySQLcreate(cpl); }
    void Read(CPL* cpl) override { MySQLread(cpl); }
    void Update(CPL* cpl) override { MySQLupdate(cpl); }
    void Remove(CPL* cpl) override { MySQLremove(cpl); }

    Query* MySQLcreate(CPL* cpl);
    Query* MySQLread(CPL* cpl);
    Query* MySQLupdate(CPL* cpl);
    Query* MySQLremove(CPL* cpl);

private:
    static std::string _database;
    static std::string _table;
};