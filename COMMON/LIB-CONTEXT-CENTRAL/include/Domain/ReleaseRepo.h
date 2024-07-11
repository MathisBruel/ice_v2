#pragma once

#include "Domain/Release.h"
class ReleaseRepo
{
public:
    virtual void Create(Releases* release) = 0;
    virtual void Read(Releases* release) = 0;
    virtual void Update(Releases* release) = 0;
    virtual void Remove(Releases* release) = 0;
};