#pragma once
#include <memory>
#include "ResultQuery.h"

class COD_LocalisationRepo
{
public:
    virtual ~COD_LocalisationRepo() = default;
    virtual std::unique_ptr<ResultQuery> getLocalisations() = 0;
    virtual std::unique_ptr<ResultQuery> getLocalisation(int id) = 0;
}; 