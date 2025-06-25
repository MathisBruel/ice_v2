#pragma once

#include <string>
#include "ContentOpsDomain/COD_Site.h"

class COB_Site : public COD_Site
{
public:
    std::string toXmlString() const;

};