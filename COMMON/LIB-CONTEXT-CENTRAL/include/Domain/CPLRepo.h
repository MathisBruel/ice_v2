#pragma once

#include "Domain/CPL.h"
class CPLRepo
{
public:
    CPLRepo();
    ~CPLRepo();

    virtual void create(CPL* cpl);
    virtual void read(CPL* cpl);
    virtual void update(CPL* cpl);
    virtual void remove(CPL* cpl);
};