#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Site.h"

class COB_Sites : public std::vector<COB_Site>
{
public:
    operator std::string() const;
};