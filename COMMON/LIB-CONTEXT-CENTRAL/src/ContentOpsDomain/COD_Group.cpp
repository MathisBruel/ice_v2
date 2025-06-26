#include "ContentOpsDomain/COD_Group.h"

COD_Group::COD_Group()
{
    this->_groupId = -1;
    this->_groupName = "";
    this->_groupParent = -1;
    this->_groupRepo = nullptr;
}

COD_Group::~COD_Group()
{
    if (this->_groupRepo != nullptr)
    {
        delete this->_groupRepo;
    }
}

void COD_Group::SetGroupId(int groupId)
{
    this->_groupId = groupId;
}

void COD_Group::SetDatas(std::string groupName, int groupParent)
{
    this->_groupName = groupName;
    this->_groupParent = groupParent;
} 