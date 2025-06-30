#pragma once
#include <memory>
#include "ContentOpsDomain/COD_TypeRepo.h"
#include "Query.h"

class MySQLTypeRepo : public COD_TypeRepo
{
public:
    // Implémentation des méthodes virtuelles pures
    std::unique_ptr<ResultQuery> getTypes() override;
    std::unique_ptr<ResultQuery> getType(int id) override;

private:
    static std::string _database;
    static std::string _table;
}; 