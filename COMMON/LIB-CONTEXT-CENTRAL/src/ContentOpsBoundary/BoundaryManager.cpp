#include "ContentOpsBoundary/BoundaryManager.h"
#include "ContentOpsBoundary/COB_Site.h"
#include "ContentOpsInfra/MySQLSiteRepo.h"

BoundaryManager::BoundaryManager()
{
    _siteRepo = new COB_SiteRepo(new MySQLSiteRepo());
}

std::string BoundaryManager::GetAllContentsAsXml() {
    throw std::logic_error("GetAllContentsAsXml not implemented");
}

std::string BoundaryManager::GetContentAsXml(int contentId) {
    throw std::logic_error("GetContentAsXml not implemented");
}

std::string BoundaryManager::GetContentReleasesAsXml(int contentId,int typeId, int localizationId) {
    throw std::logic_error("GetContentReleasesAsXml not implemented");
}

std::string BoundaryManager::GetGroupsAsXml() {
    throw std::logic_error("GetGroupAsXml not implemented");
}

std::string BoundaryManager::GetGroupAsXml(int groupeId) {
    throw std::logic_error("GetGroupAsXml not implemented");
}

std::string BoundaryManager::GetSitesAsXml() {
    try {
        std::vector<COB_Site> sites = _siteRepo->GetSites(); 
        std::string xml = "<sites>";
        
        size_t estimatedSize = sites.size() * 100; 
        xml.reserve(estimatedSize);
        
        for (const COB_Site& site : sites) { 
            xml += site.toXmlString();
        }
        xml += "</sites>";
        return xml; 
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get sites : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
    //throw std::logic_error("GetSitesAsXml not implemented");
}

std::string BoundaryManager::GetSiteCplsAsXml(int siteId) {
    throw std::logic_error("GetSiteCplsAsXml not implemented");
}

void BoundaryManager::UpdateContent(int contentId) {
    throw std::logic_error("UpdateContent not implemented");
}

void BoundaryManager::CreateContent() {
    throw std::logic_error("CreateContent not implemented");
}