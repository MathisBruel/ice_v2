#include "ContentOpsBoundary/BoundaryManager.h"
#include "ContentOpsBoundary/COB_Site.h"
#include "ContentOpsInfra/MySQLSiteRepo.h"
#include "ContentOpsBoundary/COB_Group.h"
#include "ContentOpsInfra/MySQLGroupRepo.h"
#include "ContentOpsBoundary/COB_Sites.h"
#include "ContentOpsBoundary/COB_Groups.h"
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsInfra/MySQLContentRepo.h"
#include "ContentOpsBoundary/COB_Contents.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"

BoundaryManager::BoundaryManager()
{
    _siteRepo = std::make_shared<COB_SiteRepo>(std::make_shared<MySQLSiteRepo>());
    _groupRepo = std::make_shared<COB_GroupRepo>(std::make_shared<MySQLGroupRepo>());
    _contentRepo = std::make_shared<COB_ContentRepo>(std::make_shared<MySQLContentRepo>());
    _releaseRepo = std::make_shared<COB_ReleaseRepo>(std::make_shared<MySQLReleaseRepo>());
}

BoundaryManager::~BoundaryManager()
{
}

std::string BoundaryManager::GetAllContentsAsXml() {
    try {
        COB_Contents contents = _contentRepo->GetContents(); 
        return contents; 
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get contents : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetContentAsXml(int contentId) {
    try {
        COB_Content content = _contentRepo->GetContent(contentId);
        return content;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get content : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetContentReleasesAsXml(int contentId) {
    try {
        COB_Releases releases = _releaseRepo->GetReleases(contentId);
        return releases;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get releases : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetContentReleasesAsXml(int contentId,int typeId, int localizationId) {
    throw std::logic_error("GetContentReleasesAsXml not implemented");
}

std::string BoundaryManager::GetGroupsAsXml() {
    try {
        COB_Groups groups = _groupRepo->GetGroups(); 
        return groups; 
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get groups : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetGroupAsXml(int groupeId) {
    try {
        COB_Group group = _groupRepo->GetGroup(groupeId);
        return group;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get group : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetSitesAsXml(int groupId) {
    try {
        COB_Sites sites = _siteRepo->GetSites(groupId); 
        return sites;
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