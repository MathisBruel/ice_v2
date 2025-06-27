#pragma once

#include <string>
#include "ContentOpsDomain/COD_Site.h"

class COB_Site : public COD_Site
{
public:
    COB_Site(int siteId, std::string siteName, int siteGroup, int siteConnection);
    operator std::string() const;
};