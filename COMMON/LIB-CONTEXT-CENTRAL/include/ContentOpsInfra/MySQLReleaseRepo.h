#pragma once
#include <iostream>

#include "ContentOpsDomain/COD_ReleaseRepo.h"
#include "Query.h"

class MySQLReleaseRepo : public COD_ReleaseRepo
{
public:
    void Create(COD_Releases* release) override { _query = MySQLcreate(release); }
    void Read(COD_Releases* release) override { _query = MySQLread(release); }
    void Update(COD_Releases* release) override { _query = MySQLupdate(release); }
    void Remove(COD_Releases* release) override { _query = MySQLremove(release); }

    Query* MySQLcreate(COD_Releases* release);
    Query* MySQLread(COD_Releases* release);
    Query* MySQLupdate(COD_Releases* release);
    Query* MySQLremove(COD_Releases* release);

    Query* GetQuery() { return _query; }
private:
    static std::string _database;
    static std::string _table;

    int* _releaseIds;
    std::string _CPLRefPath;

    Query* _query;
};