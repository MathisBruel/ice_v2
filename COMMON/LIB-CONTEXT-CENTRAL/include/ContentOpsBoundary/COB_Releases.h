#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Release.h"

class COB_Releases : public std::vector<COB_Release>
{
public:
    operator std::string() const;
};