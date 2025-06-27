#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Content.h"

class COB_Contents : public std::vector<COB_Content>
{
    public:
        operator std::string() const;
}; 