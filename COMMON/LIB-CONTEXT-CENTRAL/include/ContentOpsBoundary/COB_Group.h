#pragma once

#include <string>
#include "ContentOpsDomain/COD_Group.h"

class COB_Group : public COD_Group
{
public:
    std::string toXmlString() const;
}; 