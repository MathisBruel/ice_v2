#pragma once
#include "BoundaryStateManager.h"
#include <stdexcept>
#include "ContentOpsBoundary/COB_SiteRepo.h"

using namespace ContentOpsBoundaryEnum;

class BoundaryManager {
    public:
    std::string GetAllContentsAsXml();
    std::string GetContentAsXml(int contentId);
    std::string GetContentReleasesAsXml(int contentId,int typeId, int localizationId);
    std::string GetGroupsAsXml();
    std::string GetGroupAsXml(int groupeId);
    std::string GetSitesAsXml();
    std::string GetSiteCplsAsXml(int siteId);
    void UpdateContent(int contentId);
    void CreateContent();


    static BoundaryManager& GetInstance() {
        static BoundaryManager instance;
        return instance;
    }

    BoundaryManager(const BoundaryManager&) = delete;
    BoundaryManager& operator=(const BoundaryManager&) = delete;

    private:
    BoundaryManager();
    COB_SiteRepo* _siteRepo;
};