#include <iostream>

#include "CISRepo.h"
#include "Query.h"

class MySQLCISRepo : public CISRepo
{
public:
    MySQLCISRepo();
    ~MySQLCISRepo();

    Query* create(CIS* cis);
    Query* read(CIS* cis);
    Query* update(CIS* cis);
    Query* remove(CIS* cis);

private:
    static std::string database;
    static std::string table;
};