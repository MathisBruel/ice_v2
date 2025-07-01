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
#include "ContentOpsBoundary/COB_LocalisationRepo.h"
#include "ContentOpsInfra/MySQLLocalisationRepo.h"
#include "ContentOpsBoundary/COB_TypeRepo.h"
#include "ContentOpsInfra/MySQLTypeRepo.h"
#include "ContentOpsBoundary/COB_CplRepo.h"
#include "ContentOpsInfra/MySQLCplRepo.h"

BoundaryManager::BoundaryManager()
{
    _siteRepo = std::make_shared<COB_SiteRepo>(std::make_shared<MySQLSiteRepo>());
    _groupRepo = std::make_shared<COB_GroupRepo>(std::make_shared<MySQLGroupRepo>());
    _contentRepo = std::make_shared<COB_ContentRepo>(std::make_shared<MySQLContentRepo>());
    _releaseRepo = std::make_shared<COB_ReleaseRepo>(std::make_shared<MySQLReleaseRepo>());
    _localisationRepo = std::make_shared<COB_LocalisationRepo>(std::make_shared<MySQLLocalisationRepo>());
    _typeRepo = std::make_shared<COB_TypeRepo>(std::make_shared<MySQLTypeRepo>());
    _cplRepo = std::make_shared<COB_CplRepo>(std::make_shared<MySQLCplRepo>());
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
    try {
        COB_Release release = _releaseRepo->GetRelease(contentId, typeId, localizationId);
        return static_cast<std::string>(release);
    }
    catch(const std::exception& e) {
        std::string errorMsg = "Failed to get release : " + std::string(e.what());
        throw std::runtime_error(errorMsg);
    }
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
    try {
        return _cplRepo->GetCplsBySiteAsXml(siteId);
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpls for site " + std::to_string(siteId) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

void BoundaryManager::UpdateContent(int contentId) {
    throw std::logic_error("UpdateContent not implemented");
}

void BoundaryManager::CreateContent() {
    throw std::logic_error("CreateContent not implemented");
}

std::string BoundaryManager::GetLocalisationsAsXml() {
    try {
        std::vector<COB_Localisation> localisations = _localisationRepo->GetLocalisations();
        std::string xml = "<localisations>";
        for (const auto& localisation : localisations) {
            xml += static_cast<std::string>(localisation);
        }
        xml += "</localisations>";
        return xml;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get localisations : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetTypesAsXml() {
    try {
        std::vector<COB_Type> types = _typeRepo->GetTypes();
        std::string xml = "<types>";
        for (const auto& type : types) {
            xml += static_cast<std::string>(type);
        }
        xml += "</types>";
        return xml;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get types : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplsAsXml() {
    try {
        return _cplRepo->GetCplsAsXml();
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpls : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplAsXml(int id) {
    try {
        return _cplRepo->GetCplAsXml(id);
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpl " + std::to_string(id) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplAsXmlByUuid(const std::string& uuid) {
    try {
        return _cplRepo->GetCplAsXmlByUuid(uuid);
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpl with uuid " + uuid + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplsByScriptAsXml(int scriptId) {
    try {
        return _cplRepo->GetCplsByScriptAsXml(scriptId);
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpls for script " + std::to_string(scriptId) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplsByReleaseAsXml(int releaseId) {
    try {
        return _cplRepo->GetCplsByReleaseAsXml(releaseId);
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpls for release " + std::to_string(releaseId) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetUnlinkedCplsAsXml() {
    try {
        return _cplRepo->GetUnlinkedCplsAsXml();
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get unlinked cpls : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}