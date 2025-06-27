#include "ContentOpsDomain/COD_Group.h"

COD_Group::COD_Group(int groupId, std::string groupName, int groupParent)
{
    this->_groupId = groupId;
    this->_groupName = groupName;
    this->_groupParent = groupParent;
}

COD_Group::~COD_Group()
{
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