#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Group.h"

class COB_Groups : public std::vector<COB_Group>
{
    public:
        operator std::string() const;
};