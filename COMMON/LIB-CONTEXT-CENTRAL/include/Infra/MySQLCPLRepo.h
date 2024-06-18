#pragma once
#include <iostream>

#include "CPLRepo.h"
#include "Query.h"

class MySQLCPLRepo : public CPLRepo
{
public:
    MySQLCPLRepo();
    ~MySQLCPLRepo();
    
    Query* create(CPL* cpl);
    Query* read(CPL* cpl);
    Query* update(CPL* cpl);
    Query* remove(CPL* cpl);


private:
    static std::string database;
    static std::string table;
};