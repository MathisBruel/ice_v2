#pragma once

#include <memory>

class COD_Content;
class ResultQuery;

class COD_ContentRepo
{
public:
    virtual ~COD_ContentRepo() = default;
    virtual void Create(COD_Content* content) = 0;
    virtual void Read(COD_Content* content) = 0;
    virtual void Update(COD_Content* content) = 0;
    virtual void Remove(COD_Content* content) = 0;
    virtual std::unique_ptr<ResultQuery> getContents() = 0;
    virtual std::unique_ptr<ResultQuery> getContent(int contentId) = 0;
};