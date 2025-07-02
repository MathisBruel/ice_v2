#pragma once

#include <vector>
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsDomain/COD_ContentRepo.h"
#include "ContentOpsBoundary/COB_Contents.h"

class COB_ContentRepo
{
public: 
    COB_ContentRepo(std::shared_ptr<COD_ContentRepo> contentRepo);
    ~COB_ContentRepo();

    COB_Contents GetContents();
    COB_Content GetContent(int contentId);
    std::unique_ptr<COB_Content> Create(const std::string& title);

private:
    std::shared_ptr<COD_ContentRepo> _contentRepo;
}; 