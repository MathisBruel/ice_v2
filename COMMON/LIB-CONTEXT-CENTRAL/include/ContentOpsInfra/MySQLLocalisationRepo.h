#pragma once
#include <memory>
#include "ContentOpsDomain/COD_LocalisationRepo.h"
#include "Query.h"

class MySQLLocalisationRepo : public COD_LocalisationRepo
{
public:
    // Implémentation des méthodes virtuelles pures
    std::unique_ptr<ResultQuery> getLocalisations() override;
    std::unique_ptr<ResultQuery> getLocalisation(int id) override;

private:
    static std::string _database;
    static std::string _table;
}; 