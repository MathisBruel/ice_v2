#pragma once
#include <memory>
#include <vector>
#include "ContentOpsDomain/COD_TypeRepo.h"
#include "ContentOpsBoundary/COB_Type.h"
#include "ContentOpsBoundary/COB_Types.h"

class COB_TypeRepo
{
public:
    COB_TypeRepo(std::shared_ptr<COD_TypeRepo> typeRepo);
    ~COB_TypeRepo();

    COB_Types GetTypes();
    COB_Type GetType(int id);

private:
    std::shared_ptr<COD_TypeRepo> _typeRepo;
}; 