#pragma once

class COD_Group;
class ResultQuery;

class COD_GroupRepo
{
public:
    virtual void Create(COD_Group* group) = 0;
    virtual void Read(COD_Group* group) = 0;
    virtual void Update(COD_Group* group) = 0;
    virtual void Remove(COD_Group* group) = 0;
    virtual ResultQuery* getGroups() = 0;
    virtual ResultQuery* getGroup(int groupId) = 0;
}; 