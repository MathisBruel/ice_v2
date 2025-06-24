#pragma once

class COD_Sync;
class COD_SyncRepo
{
public:
    virtual void Create(COD_Sync* sync) = 0;
    virtual void Read(COD_Sync* sync) = 0;
    virtual void Update(COD_Sync* sync) = 0;
    virtual void Remove(COD_Sync* sync) = 0;
};