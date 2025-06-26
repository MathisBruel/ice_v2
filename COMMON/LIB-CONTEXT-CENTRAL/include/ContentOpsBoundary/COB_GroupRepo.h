#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Group.h"
#include "ContentOpsDomain/COD_GroupRepo.h"

class COB_GroupRepo
{
public: 
    COB_GroupRepo(COD_GroupRepo* groupRepo);
    ~COB_GroupRepo();

    std::vector<COB_Group> GetGroups();
    COB_Group GetGroup(int groupId);

private:
    COD_GroupRepo* _groupRepo;
}; 