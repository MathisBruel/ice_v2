#pragma once

#include <string>
#include "ContentOpsDomain/COD_Group.h"

class COB_Group : public COD_Group
{
public:
    COB_Group(int groupId, std::string groupName, int groupParent);
    operator std::string() const;
}; 