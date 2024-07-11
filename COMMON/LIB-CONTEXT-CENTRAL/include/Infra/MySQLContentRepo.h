#pragma once
#include <iostream>

#include "Domain/ContentRepo.h"
#include "Query.h"

class MySQLContentRepo : public ContentRepo
{
public:
    void Create(Content* content) override { MySQLcreate(content); }
    void Read(Content* content) override { MySQLread(content); }
    void Update(Content* content) override { MySQLupdate(content); }
    void Remove(Content* content) override { MySQLremove(content); }

    Query* MySQLcreate(Content* content);
    Query* MySQLread(Content* content);
    Query* MySQLupdate(Content* content);
    Query* MySQLremove(Content* content);

private:
    static std::string _database;
    static std::string _table;
};