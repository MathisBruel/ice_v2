#pragma once

#include "ContentOpsDomain/COD_Release.h"
class COD_ReleaseRepo
{
public:
    virtual void Create(COD_Releases* release) = 0;
    virtual void Read(COD_Releases* release) = 0;
    virtual void Update(COD_Releases* release) = 0;
    virtual void Remove(COD_Releases* release) = 0;
};