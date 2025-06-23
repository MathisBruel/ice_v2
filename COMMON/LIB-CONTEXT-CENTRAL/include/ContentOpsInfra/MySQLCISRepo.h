#pragma once
#include <iostream>

#include "ContentOpsDomain/CISRepo.h"
#include "Query.h"

class MySQLCISRepo : public CISRepo
{
public:
    void Create(CIS* cis) override { _query = MySQLcreate(cis); }
    void Read(CIS* cis) override { _query = MySQLread(cis); }
    void Update(CIS* cis) override { _query = MySQLupdate(cis); }
    void Remove(CIS* cis) override { _query = MySQLremove(cis); }

    Query* MySQLcreate(CIS* cis);
    Query* MySQLread(CIS* cis);
    Query* MySQLupdate(CIS* cis);
    Query* MySQLremove(CIS* cis);

    Query* GetQuery() { return _query; }
private:
    static std::string _database;
    static std::string _table;

    int* _CISids;
    std::string _pathCIS;

    Query* _query;
};