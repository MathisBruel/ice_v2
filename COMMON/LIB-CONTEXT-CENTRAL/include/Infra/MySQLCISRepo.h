#pragma once
#include <iostream>

#include "Domain/CISRepo.h"
#include "Query.h"

class MySQLCISRepo : public CISRepo
{
public:
    MySQLCISRepo();
    ~MySQLCISRepo();

    void create(CIS* cis) { MySQLcreate(cis); }
    void read(CIS* cis) { MySQLread(cis); }
    void update(CIS* cis) { MySQLupdate(cis); }
    void remove(CIS* cis) { MySQLremove(cis); }

    Query* MySQLcreate(CIS* cis);
    Query* MySQLread(CIS* cis);
    Query* MySQLupdate(CIS* cis);
    Query* MySQLremove(CIS* cis);

private:
    static std::string database;
    static std::string table;
};