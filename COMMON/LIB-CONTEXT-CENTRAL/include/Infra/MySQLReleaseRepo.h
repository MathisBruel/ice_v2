#pragma once
#include <iostream>

#include "Domain/ReleaseRepo.h"
#include "Query.h"

class MySQLReleaseRepo : public ReleaseRepo
{
public:
    MySQLReleaseRepo();
    ~MySQLReleaseRepo();

    void create(Releases* release) { MySQLcreate(release); }
    void read(Releases* release) { MySQLread(release); }
    void update(Releases* release) { MySQLupdate(release); }
    void remove(Releases* release) { MySQLremove(release); }

    Query* MySQLcreate(Releases* release);
    Query* MySQLread(Releases* release);
    Query* MySQLupdate(Releases* release);
    Query* MySQLremove(Releases* release);

private:
    static std::string database;
    static std::string table;
};