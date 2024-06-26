#pragma once

#include "Domain/Release.h"
class ReleaseRepo
{
public:
    ReleaseRepo();
    ~ReleaseRepo();
    virtual void create(Releases* release);
    virtual void read(Releases* release);
    virtual void update(Releases* release);
    virtual void remove(Releases* release);
};