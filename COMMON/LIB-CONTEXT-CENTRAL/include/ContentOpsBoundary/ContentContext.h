#pragma once
#include <memory>
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsBoundary/COB_ContentRepo.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"

struct ContentContext {
    std::shared_ptr<bool> isNewContent;
    std::shared_ptr<COB_Content> content;
    std::shared_ptr<std::string> pendingTitle;
    std::shared_ptr<COB_ContentRepo> contentRepo;
    std::shared_ptr<COB_ReleaseRepo> releaseRepo;
}; 