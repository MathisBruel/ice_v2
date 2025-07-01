#pragma once

#include <vector>
#include <string>
#include "ContentOpsBoundary/COB_Localisation.h"

class COB_Localisations : public std::vector<COB_Localisation>
{
public:
    operator std::string() const;
};