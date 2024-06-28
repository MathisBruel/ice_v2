#pragma once
#include <iostream>

#include "Domain/ReleaseRepo.h"
#include "Query.h"

class MySQLReleaseRepo : public ReleaseRepo
{
public:
    MySQLReleaseRepo();
    ~MySQLReleaseRepo();

    void Create(Releases* release) override { MySQLcreate(release); }
    void Read(Releases* release) override { MySQLread(release); }
    void Update(Releases* release) override { MySQLupdate(release); }
    void Remove(Releases* release) override { MySQLremove(release); }

    Query* MySQLcreate(Releases* release);
    Query* MySQLread(Releases* release);
    Query* MySQLupdate(Releases* release);
    Query* MySQLremove(Releases* release);

private:
    static std::string _database;
    static std::string _table;
};