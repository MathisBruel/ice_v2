#pragma once
#include <iostream>

#include "Domain/CPLRepo.h"
#include "Query.h"

class MySQLCPLRepo : public CPLRepo
{
public:
    MySQLCPLRepo();
    ~MySQLCPLRepo();
    
    void create(CPL* cpl) { MySQLcreate(cpl); }
    void read(CPL* cpl) { MySQLread(cpl); }
    void update(CPL* cpl) { MySQLupdate(cpl); }
    void remove(CPL* cpl) { MySQLremove(cpl); }

    Query* MySQLcreate(CPL* cpl);
    Query* MySQLread(CPL* cpl);
    Query* MySQLupdate(CPL* cpl);
    Query* MySQLremove(CPL* cpl);

private:
    static std::string database;
    static std::string table;
};