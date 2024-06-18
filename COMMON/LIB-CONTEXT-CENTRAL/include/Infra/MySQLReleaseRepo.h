#pragma once
#include <iostream>

#include "ReleaseRepo.h"
#include "Query.h"

class MySQLReleaseRepo : public ReleaseRepo
{
public:
    MySQLReleaseRepo();
    ~MySQLReleaseRepo();

    Query* create(Release* release);
    Query* read(Release* release);
    Query* update(Release* release);
    Query* remove(Release* release);

private:
    static std::string database;
    static std::string table;
};