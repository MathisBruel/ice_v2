#include "ContentOpsBoundary/COB_GroupRepo.h"
#include <stdexcept>
#include <utility> // Pour std::move
#include "ContentOpsBoundary/COB_Group.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLGroupRepo.h"

COB_GroupRepo::COB_GroupRepo(COD_GroupRepo* groupRepo)
{
    _groupRepo = groupRepo;
}

COB_GroupRepo::~COB_GroupRepo()
{
    delete _groupRepo;
}

std::vector<COB_Group> COB_GroupRepo::GetGroups()
{
    std::vector<COB_Group> groups;
    ResultQuery* result = _groupRepo->getGroups();
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
            groups.emplace_back();
            COB_Group& group = groups.back();
            group.SetGroupId(*id);
            int parentId = (parent != nullptr) ? *parent : -1;
            group.SetDatas(*name, parentId); 
        }
    }
    delete result;
    return std::move(groups); 
}

COB_Group COB_GroupRepo::GetGroup(int groupId)
{
    ResultQuery* result = _groupRepo->getGroup(groupId);
    if (!result || !result->isValid()) {
        if (result) delete result;
        throw std::runtime_error("Failed to get group with id " + std::to_string(groupId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    
    if (result->getNbRows() == 0) {
        delete result;
        throw std::runtime_error("Group with id " + std::to_string(groupId) + " not found");
    }
    
    COB_Group group;
    
    int* id = result->getIntValue(0, "id_group");
    std::string* name = result->getStringValue(0, "name");
    int* parent = result->getIntValue(0, "id_group_1");
    
    if (id && name) {
        group.SetGroupId(*id);
        int parentId = (parent != nullptr) ? *parent : -1;
        group.SetDatas(*name, parentId);
    } else {
        delete result;
        throw std::runtime_error("Invalid data received for group " + std::to_string(groupId));
    }
    
    delete result;
    return group;
} 