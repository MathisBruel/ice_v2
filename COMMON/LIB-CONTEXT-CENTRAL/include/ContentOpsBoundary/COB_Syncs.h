#pragma once
#include <vector>
#include "ContentOpsBoundary/COB_Sync.h"

class COB_Syncs : public std::vector<COB_Sync>
{
public:
    operator std::string() const;
}; 