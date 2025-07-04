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
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"
#include "ContentOpsBoundary/COB_LocalisationRepo.h"
#include "ContentOpsInfra/MySQLLocalisationRepo.h"
#include "ContentOpsBoundary/COB_TypeRepo.h"
#include "ContentOpsInfra/MySQLTypeRepo.h"
#include "ContentOpsBoundary/COB_CplRepo.h"
#include "ContentOpsInfra/MySQLCplRepo.h"
#include "ContentOpsBoundary/COB_Cpl.h"
#include "ContentOpsBoundary/BoundaryStateManager.h"
#include <stdexcept>

BoundaryManager::BoundaryManager()
{
    _configurator = std::make_shared<COB_Configurator>();
}

BoundaryManager::~BoundaryManager()
{
}

std::string BoundaryManager::GetAllContentsAsXml() {
    try {
        COB_Contents contents = _configurator->GetContentRepo()->GetContents(); 
        return contents; 
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get contents : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetContentAsXml(int contentId) {
    try {
        COB_Content content = _configurator->GetContentRepo()->GetContent(contentId);
        return content;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get content : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetContentReleasesAsXml(int contentId) {
    try {
        COB_Releases releases = _configurator->GetReleaseRepo()->GetReleases(contentId);
        return releases;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get releases : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetContentReleasesAsXml(int contentId,int typeId, int localizationId) {
    try {
        COB_Release release = _configurator->GetReleaseRepo()->GetRelease(contentId, typeId, localizationId);
        return static_cast<std::string>(release);
    }
    catch(const std::exception& e) {
        std::string errorMsg = "Failed to get release : " + std::string(e.what());
        throw std::runtime_error(errorMsg);
    }
}

std::string BoundaryManager::GetGroupsAsXml() {
    try {
        COB_Groups groups = _configurator->GetGroupRepo()->GetGroups(); 
        return groups; 
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get groups : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetGroupAsXml(int groupeId) {
    try {
        COB_Group group = _configurator->GetGroupRepo()->GetGroup(groupeId);
        return group;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get group : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetSitesAsXml(int groupId) {
    try {
        COB_Sites sites = _configurator->GetSiteRepo()->GetSites(groupId); 
        return sites;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get sites : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetSiteCplsAsXml(int siteId) {
    try {
        COB_Cpls cpls = _configurator->GetCplRepo()->GetCplsBySite(siteId);
        return cpls;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpls for site " + std::to_string(siteId) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

void BoundaryManager::UpdateContent(int contentId) {
    throw std::logic_error("UpdateContent not implemented");
}

TransitionResponse BoundaryManager::CreateContent(std::string title) {
    try {
        return _boundaryStateManager.CreateContent(title);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors de la création du content : ") + e.what());
    }
}

std::string BoundaryManager::GetLocalisationsAsXml() {
    try {
        COB_Localisations localisations = _configurator->GetLocalisationRepo()->GetLocalisations();
        return localisations;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get localisations : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetTypesAsXml() {
    try {
        COB_Types types = _configurator->GetTypeRepo()->GetTypes();
        return types;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get types : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplsAsXml() {
    try {
        COB_Cpls cpls = _configurator->GetCplRepo()->GetCpls();
        return cpls;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpls : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplAsXml(int id) {
    try {
        COB_Cpl cpl = _configurator->GetCplRepo()->GetCpl(id);
        return cpl;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpl " + std::to_string(id) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplAsXmlByUuid(const std::string& uuid) {
    try {
        COB_Cpl cpl = _configurator->GetCplRepo()->GetCplByUuid(uuid);
        return cpl;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpl with uuid " + uuid + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetCplsByScriptAsXml(int scriptId) {
    throw std::logic_error("GetCplsByScriptAsXml not implemented");
}

std::string BoundaryManager::GetCplsByReleaseAsXml(int releaseId) {
    try {
        COB_Cpls cpls = _configurator->GetCplRepo()->GetCplsByRelease(releaseId);
        return cpls;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get cpls for release " + std::to_string(releaseId) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetUnlinkedCplsAsXml() {
    try {
        COB_Cpls cpls = _configurator->GetCplRepo()->GetUnlinkedCpls();
        return cpls;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get unlinked cpls : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetReleaseCplsAsXml(int contentId, int typeId, int localisationId) {
    try {
        COB_Cpls cpls = _configurator->GetCplRepo()->GetCplsByRelease(contentId, typeId, localisationId);
        return cpls;
    }
    catch(const std::exception& e) {
        std::string errorMsg = "Failed to get CPLs for release : " + std::string(e.what());
        throw std::runtime_error(errorMsg);
    }
}

// Implémentation des méthodes de suppression pour StatePublishing
void BoundaryManager::DeleteRelease(int contentId, int typeId, int localisationId) {
    try {
        auto releaseRepo = _configurator->GetReleaseRepo();
        auto release = releaseRepo->GetRelease(contentId, typeId, localisationId);
        // Créer un pointeur pour la suppression
        COB_Release* releasePtr = new COB_Release(release);
        releaseRepo->Remove(releasePtr);
        delete releasePtr;
        
        // Suppression du content associé si nécessaire
        auto content = _configurator->GetContentRepo()->GetContent(contentId);
        std::string releaseId = std::to_string(contentId) + "_" + std::to_string(typeId) + "_" + std::to_string(localisationId);
        content.DeleteRelease(releaseId);
    }
    catch(const std::exception& e) {
        std::string errorMsg = "Failed to delete release : " + std::string(e.what());
        throw std::runtime_error(errorMsg);
    }
}

void BoundaryManager::DeleteCPL(int contentId, int typeId, int localisationId, int servPairConfigId) {
    try {
        auto releaseRepo = _configurator->GetReleaseRepo();
        auto release = releaseRepo->GetRelease(contentId, typeId, localisationId);
        std::string compositeId = std::to_string(servPairConfigId) + "_" + std::to_string(contentId) + "_" + std::to_string(typeId) + "_" + std::to_string(localisationId);
        auto cpl = release.GetCPL(compositeId);
        if (cpl) {
            auto cplRepo = _configurator->GetCplRepo();
            // Créer un pointeur COB_Cpl pour la suppression
            COB_Cpl* cplPtr = new COB_Cpl();
            // Copier les données du CPL si nécessaire
            cplRepo->Remove(cplPtr);
            delete cplPtr;
            release.DeleteCPL(compositeId);
        }
    }
    catch(const std::exception& e) {
        std::string errorMsg = "Failed to delete CPL : " + std::string(e.what());
        throw std::runtime_error(errorMsg);
    }
}

void BoundaryManager::DeleteSync(int contentId, int typeId, int localisationId, int servPairConfigId) {
    try {
        auto releaseRepo = _configurator->GetReleaseRepo();
        auto release = releaseRepo->GetRelease(contentId, typeId, localisationId);
        std::string compositeId = std::to_string(servPairConfigId) + "_" + std::to_string(contentId) + "_" + std::to_string(typeId) + "_" + std::to_string(localisationId);
        auto cpl = release.GetCPL(compositeId);
        if (cpl && cpl->GetSync()) {
            // Suppression du sync directement depuis le CPL
            cpl->DeleteSync();
        }
    }
    catch(const std::exception& e) {
        std::string errorMsg = "Failed to delete Sync : " + std::string(e.what());
        throw std::runtime_error(errorMsg);
    }
}

void BoundaryManager::DeleteSyncLoop(int contentId, int typeId, int localisationId, int servPairConfigId) {
    try {
        auto releaseRepo = _configurator->GetReleaseRepo();
        auto release = releaseRepo->GetRelease(contentId, typeId, localisationId);
        std::string compositeId = std::to_string(servPairConfigId) + "_" + std::to_string(contentId) + "_" + std::to_string(typeId) + "_" + std::to_string(localisationId);
        auto syncLoop = release.GetSyncLoop(compositeId);
        if (syncLoop) {
            // Suppression du syncLoop directement depuis le release
            release.DeleteSyncLoop(compositeId);
        }
    }
    catch(const std::exception& e) {
        std::string errorMsg = "Failed to delete SyncLoop : " + std::string(e.what());
        throw std::runtime_error(errorMsg);
    }
}