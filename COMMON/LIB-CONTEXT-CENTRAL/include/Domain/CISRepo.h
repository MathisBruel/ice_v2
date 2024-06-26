#pragma once

#include "Domain/CIS.h"
class CISRepo
{
public:
    CISRepo();
    ~CISRepo();
    virtual void create(CIS* cis);
    virtual void read(CIS* cis);
    virtual void update(CIS* cis);
    virtual void remove(CIS* cis);
};