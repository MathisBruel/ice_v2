#pragma once
#include <iostream>

#include "ContentOpsDomain/ReleaseRepo.h"
#include "Query.h"

class MySQLReleaseRepo : public ReleaseRepo
{
public:
    void Create(Releases* release) override { _query = MySQLcreate(release); }
    void Read(Releases* release) override { _query = MySQLread(release); }
    void Update(Releases* release) override { _query = MySQLupdate(release); }
    void Remove(Releases* release) override { _query = MySQLremove(release); }

    Query* MySQLcreate(Releases* release);
    Query* MySQLread(Releases* release);
    Query* MySQLupdate(Releases* release);
    Query* MySQLremove(Releases* release);

    Query* GetQuery() { return _query; }
private:
    static std::string _database;
    static std::string _table;

    int* _releaseIds;
    std::string _CPLRefPath;

    Query* _query;
};