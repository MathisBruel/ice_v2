#pragma once

class COD_Content;
class COD_ContentRepo
{
public:
    virtual void Create(COD_Content* content) = 0;
    virtual void Read(COD_Content* content) = 0;
    virtual void Update(COD_Content* content) = 0;
    virtual void Remove(COD_Content* content) = 0;
};