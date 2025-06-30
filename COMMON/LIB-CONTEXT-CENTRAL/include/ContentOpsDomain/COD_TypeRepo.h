#pragma once
#include <memory>
#include "ResultQuery.h"

class COD_TypeRepo
{
public:
    virtual ~COD_TypeRepo() = default;
    virtual std::unique_ptr<ResultQuery> getTypes() = 0;
    virtual std::unique_ptr<ResultQuery> getType(int id) = 0;
}; 