#pragma once

#include "ContentOpsDomain/COD_Site.h"

class COB_Site : public COD_Site
{
public:
    COB_Site();
    ~COB_Site();

    std::string toXmlString(bool printChild);
};