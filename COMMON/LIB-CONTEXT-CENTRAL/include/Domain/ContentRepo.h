#pragma once

class Content;
class ContentRepo
{
public:
    virtual void Create(Content* content) = 0;
    virtual void Read(Content* content) = 0;
    virtual void Update(Content* content) = 0;
    virtual void Remove(Content* content) = 0;
};