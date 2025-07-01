#pragma once

#include <vector>
#include <string>
#include "ContentOpsBoundary/COB_Type.h"

class COB_Types : public std::vector<COB_Type>
{
public:
    operator std::string() const;
};