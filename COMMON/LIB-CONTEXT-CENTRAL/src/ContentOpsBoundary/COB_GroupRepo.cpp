#include "ContentOpsBoundary/COB_GroupRepo.h"
#include "ContentOpsBoundary/COB_Groups.h"
#include <stdexcept>
#include <utility> // Pour std::move
#include "ContentOpsBoundary/COB_Group.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLGroupRepo.h"

COB_GroupRepo::COB_GroupRepo(std::shared_ptr<COD_GroupRepo> groupRepo)
{
    _groupRepo = groupRepo;
}

COB_GroupRepo::~COB_GroupRepo()
{
}

COB_Groups COB_GroupRepo::GetGroups()
{
    COB_Groups groups;
    std::unique_ptr<ResultQuery> result = _groupRepo->getGroups();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get groups : " + std::string(result->getErrorMessage())); 
    }
    int nbRows = result->getNbRows();
    groups.reserve(nbRows);
    
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_group");
        std::string* name = result->getStringValue(i, "name");
        int* parent = result->getIntValue(i, "id_group_1");
        if (id && name) {
            int parentValue = parent ? *parent : -1; 
            COB_Group group(*id, *name, parentValue);
            groups.emplace_back(group);
        } 
    }
    return std::move(groups); 
}

COB_Group COB_GroupRepo::GetGroup(int groupId)
{
    std::unique_ptr<ResultQuery> result = _groupRepo->getGroup(groupId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get group with id " + std::to_string(groupId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    
    if (result->getNbRows() == 0) {
        throw std::runtime_error("Group with id " + std::to_string(groupId) + " not found");
    }
    
    int* id = result->getIntValue(0, "id_group");
    std::string* name = result->getStringValue(0, "name");
    int* parent = result->getIntValue(0, "id_group_1");

    int parentValue = parent ? *parent : -1;
    
    return std::move(COB_Group(*id, *name, parentValue));
} 