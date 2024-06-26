#pragma once

#include "Domain/Content.h"
class ContentRepo
{
public:
    ContentRepo();
    ~ContentRepo();

    virtual void create(Content* content);
    virtual void read(Content* content);
    virtual void update(Content* content);
    virtual void remove(Content* content);
};