#pragma once

#include "Domain/CPL.h"
class CPLRepo
{
public:
    CPLRepo();
    ~CPLRepo();

    virtual void Create(CPL* cpl) = 0;
    virtual void Read(CPL* cpl) = 0;
    virtual void Update(CPL* cpl) = 0;
    virtual void Remove(CPL* cpl) = 0;
};