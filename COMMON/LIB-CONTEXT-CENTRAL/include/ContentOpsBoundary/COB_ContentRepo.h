#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsDomain/COD_ContentRepo.h"
#include "ContentOpsBoundary/COB_Contents.h"
#include "ContentOpsInfra/MySQLContentRepo.h"

class COB_ContentRepo
{
public: 
    COB_ContentRepo(std::shared_ptr<COD_ContentRepo> contentRepo);
    COB_ContentRepo() : COB_ContentRepo(std::make_shared<MySQLContentRepo>()) {}
    ~COB_ContentRepo();

    COB_Contents GetContents();
    COB_Content GetContent(int contentId);
    std::unique_ptr<COB_Content> Create(const std::string& title);
    void Remove(COB_Content* content);
    void Update(COB_Content* content);
    Query* GetQuery() const;

private:
    std::shared_ptr<COD_ContentRepo> _contentRepo;
}; 