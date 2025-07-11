#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_ServerPair.h"
 
class COB_ServerPairs : public std::vector<COB_ServerPair>
{
public:
    operator std::string() const;
}; 