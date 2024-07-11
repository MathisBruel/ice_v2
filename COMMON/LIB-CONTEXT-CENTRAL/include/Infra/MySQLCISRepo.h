#pragma once
#include <iostream>

#include "Domain/CISRepo.h"
#include "Query.h"

class MySQLCISRepo : public CISRepo
{
public:
    void Create(CIS* cis) override { MySQLcreate(cis); }
    void Read(CIS* cis) override { MySQLread(cis); }
    void Update(CIS* cis) override { MySQLupdate(cis); }
    void Remove(CIS* cis) override { MySQLremove(cis); }

    Query* MySQLcreate(CIS* cis);
    Query* MySQLread(CIS* cis);
    Query* MySQLupdate(CIS* cis);
    Query* MySQLremove(CIS* cis);

private:
    static std::string _database;
    static std::string _table;
};