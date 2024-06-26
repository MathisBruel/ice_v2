#pragma once
#include <iostream>

#include "Domain/ContentRepo.h"
#include "Query.h"

class MySQLContentRepo : public ContentRepo
{
public:
    MySQLContentRepo();
    ~MySQLContentRepo();

    void create(Content* content) { MySQLcreate(content); }
    void read(Content* content) { MySQLread(content); }
    void update(Content* content) { MySQLupdate(content); }
    void remove(Content* content) { MySQLremove(content); }

    Query* MySQLcreate(Content* content);
    Query* MySQLread(Content* content);
    Query* MySQLupdate(Content* content);
    Query* MySQLremove(Content* content);

private:
    static std::string database;
    static std::string table;
};