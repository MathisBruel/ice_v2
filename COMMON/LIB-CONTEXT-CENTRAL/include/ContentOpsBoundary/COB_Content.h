#pragma once

#include <string>
#include "ContentOpsDomain/COD_Content.h"

class COB_Content : public COD_Content
{
public:
    COB_Content(int contentId, std::string contentTitle);
    COB_Content(std::string contentTitle);
    operator std::string() const;
}; 