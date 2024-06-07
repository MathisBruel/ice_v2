#include <iostream>

#include "CPLRepo.h"
#include "Query.h"

class MySQLContentRepo : public ContentRepo
{
public:
    MySQLContentRepo();
    ~MySQLContentRepo();

    Query* create(Content* content);
    Query* read(Content* content);
    Query* update(Content* content);
    Query* remove(Content* content);

private:
    static std::string database;
    static std::string table;
};