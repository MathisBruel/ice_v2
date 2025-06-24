#pragma once
#include <iostream>

#include "ContentOpsDomain/CISRepo.h"
#include "Query.h"

class MySQLCISRepo : public CISRepo
{
public:
    void Create(COD_CIS* cis) override { _query = MySQLcreate(cis); }
    void Read(COD_CIS* cis) override { _query = MySQLread(cis); }
    void Update(COD_CIS* cis) override { _query = MySQLupdate(cis); }
    void Remove(COD_CIS* cis) override { _query = MySQLremove(cis); }

    Query* MySQLcreate(COD_CIS* cis);
    Query* MySQLread(COD_CIS* cis);
    Query* MySQLupdate(COD_CIS* cis);
    Query* MySQLremove(COD_CIS* cis);

    Query* GetQuery() { return _query; }
private:
    static std::string _database;
    static std::string _table;

    int* _CISids;
    std::string _pathCIS;

    Query* _query;
};