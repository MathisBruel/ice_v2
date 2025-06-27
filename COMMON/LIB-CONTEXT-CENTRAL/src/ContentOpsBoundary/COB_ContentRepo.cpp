#include "ContentOpsBoundary/COB_ContentRepo.h"
#include "ContentOpsBoundary/COB_Contents.h"
#include <stdexcept>
#include <utility> // Pour std::move
#include "ContentOpsBoundary/COB_Content.h"
#include "ResultQuery.h"
#include "ContentOpsInfra/MySQLContentRepo.h"

COB_ContentRepo::COB_ContentRepo(std::shared_ptr<COD_ContentRepo> contentRepo)
{
    _contentRepo = contentRepo;
}

COB_ContentRepo::~COB_ContentRepo()
{
}

COB_Contents COB_ContentRepo::GetContents()
{
    COB_Contents contents;
    std::unique_ptr<ResultQuery> result = _contentRepo->getContents();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get contents : " + std::string(result->getErrorMessage())); 
    }
    int nbRows = result->getNbRows();
    contents.reserve(nbRows);
    
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_content");
        std::string* title = result->getStringValue(i, "title");
        if (id && title) {
            COB_Content content(*id, *title);
            contents.emplace_back(content);
        } 
    }
    return std::move(contents); 
}

COB_Content COB_ContentRepo::GetContent(int contentId)
{
    std::unique_ptr<ResultQuery> result = _contentRepo->getContent(contentId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get content with id " + std::to_string(contentId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    
    if (result->getNbRows() == 0) {
        throw std::runtime_error("Content with id " + std::to_string(contentId) + " not found");
    }
    
    int* id = result->getIntValue(0, "id_content");
    std::string* title = result->getStringValue(0, "title");
    
    return std::move(COB_Content(*id, *title));
} 