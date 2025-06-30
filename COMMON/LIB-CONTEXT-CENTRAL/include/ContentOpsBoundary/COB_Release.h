#pragma once

#include <string>
#include "ContentOpsDomain/COD_Release.h"

class COB_Release : public COD_Releases
{
public:
    COB_Release(int id_content, int typeMovie, int localisationMovie);
    operator std::string() const;
};