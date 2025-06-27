#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Group.h"
#include "ContentOpsDomain/COD_GroupRepo.h"
#include "ContentOpsBoundary/COB_Groups.h"

class COB_GroupRepo
{
public: 
    COB_GroupRepo(std::shared_ptr<COD_GroupRepo> groupRepo);
    ~COB_GroupRepo();

    COB_Groups GetGroups();
    COB_Group GetGroup(int groupId);

private:
    std::shared_ptr<COD_GroupRepo> _groupRepo;
}; 