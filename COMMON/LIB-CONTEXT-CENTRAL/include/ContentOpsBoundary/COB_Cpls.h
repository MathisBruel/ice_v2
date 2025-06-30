#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Cpl.h"
 
class COB_Cpls : public std::vector<COB_Cpl>
{
public:
    operator std::string() const;
}; 