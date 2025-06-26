#include "ContentOpsBoundary/BoundaryManager.h"
#include "ContentOpsBoundary/COB_Site.h"
#include "ContentOpsInfra/MySQLSiteRepo.h"
#include "ContentOpsBoundary/COB_Group.h"
#include "ContentOpsInfra/MySQLGroupRepo.h"

BoundaryManager::BoundaryManager()
{
    _siteRepo = new COB_SiteRepo(new MySQLSiteRepo());
    _groupRepo = new COB_GroupRepo(new MySQLGroupRepo());
}

BoundaryManager::~BoundaryManager()
{
    delete _siteRepo;
    delete _groupRepo;
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
    try {
        std::vector<COB_Group> groups = _groupRepo->GetGroups(); 
        std::string xml = "<groups>";
        
        size_t estimatedSize = groups.size() * 100; 
        xml.reserve(estimatedSize);
        
        for (const COB_Group& group : groups) { 
            xml += group.toXmlString();
        }
        xml += "</groups>";
        return xml; 
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get groups : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetGroupAsXml(int groupeId) {
    try {
        COB_Group group = _groupRepo->GetGroup(groupeId);
        std::string xml = "<groups>";
        xml += group.toXmlString();
        xml += "</groups>";
        return xml;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get group : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetSitesAsXml(int groupId) {
    try {
        std::vector<COB_Site> sites = _siteRepo->GetSites(groupId); 
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