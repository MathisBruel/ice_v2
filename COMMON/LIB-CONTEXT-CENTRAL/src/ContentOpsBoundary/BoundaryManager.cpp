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
#include "ContentOpsBoundary/COB_ServerPairRepo.h"
#include "ContentOpsInfra/MySQLServerPairRepo.h"
#include "ContentOpsBoundary/COB_ServerPair.h"
#include "ContentOpsBoundary/COB_ServerPairs.h"
#include "ContentOpsBoundary/COB_SyncRepo.h"
#include "ContentOpsInfra/MySQLSyncRepo.h"
#include "ContentOpsBoundary/COB_Sync.h"
#include "ContentOpsBoundary/COB_Syncs.h"
#include "ContentOpsBoundary/BoundaryStateManager.h"
#include <stdexcept>

BoundaryManager::BoundaryManager()
{
    _configurator = std::make_shared<COB_Configurator>();
}

BoundaryManager::~BoundaryManager()
{
}

std::unique_ptr<COB_Content> BoundaryManager::CreateContent(const std::string& contentTitle) {
    try {
        // Créer le content via le repository qui gère l'ID automatiquement
        return _configurator->GetContentRepo()->Create(contentTitle);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors de la création du content : ") + e.what());
    }
}

TransitionResponse BoundaryManager::CreateRelease(int contentId, int typeId, int localisationId, std::string releaseCplRefPath) {
    try {
        return _boundaryStateManager.CreateRelease(contentId, typeId, localisationId, releaseCplRefPath);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors de la création de la release : ") + e.what());
    }
}

TransitionResponse BoundaryManager::StartReleaseStateMachine(int contentId, int typeId, int localisationId) {
    try {
        return _boundaryStateManager.StartReleaseStateMachine(contentId, typeId, localisationId);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors du démarrage de la state machine : ") + e.what());
    }
}

std::string BoundaryManager::GetReleaseState(int contentId, int typeId, int localisationId) {
    try {
        return _boundaryStateManager.GetReleaseState(contentId, typeId, localisationId);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors de la récupération de l'état de la release : ") + e.what());
    }
}

void BoundaryManager::ProcessUploadCIS(int contentId, int typeId, int localisationId, std::string releaseCisPath) {
    try {
        _boundaryStateManager.ProcessUploadCIS(contentId, typeId, localisationId, releaseCisPath);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors du traitement de l'upload CIS : ") + e.what());
    }
}

void BoundaryManager::ProcessUploadSync(int contentId, int typeId, int localisationId, int servPairConfigId, std::string syncPath) {
    try {
        _boundaryStateManager.ProcessUploadSync(contentId, typeId, localisationId, servPairConfigId, syncPath);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors du traitement de l'upload Sync : ") + e.what());
    }
}

void BoundaryManager::ProcessNewCPL(int contentId, int typeId, int localisationId) {
    try {
        _boundaryStateManager.ProcessNewCPL(contentId, typeId, localisationId);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors du traitement du nouveau CPL : ") + e.what());
    }
}

void BoundaryManager::ProcessCloseRelease(int contentId, int typeId, int localisationId) {
    try {
        _boundaryStateManager.ProcessCloseRelease(contentId, typeId, localisationId);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors de la fermeture de la release : ") + e.what());
    }
}

void BoundaryManager::InitReleaseStateMachines() {
    try {
        _boundaryStateManager.InitReleaseStateMachines();
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors de l'initialisation des state machines : ") + e.what());
    }
}

std::string BoundaryManager::GetAllContentsAsXml() {
    try {
        COB_Contents contents = _configurator->GetContentRepo()->GetContents();

        std::for_each(contents.begin(), contents.end(), [this](const COB_Content& content) {
            int contentId = content.GetContentId();
            COB_Releases releases = _configurator->GetReleaseRepo()->GetReleases(contentId);
            std::for_each(releases.begin(), releases.end(), [this, contentId](const COB_Release& release) {
                const int* ids = release.GetReleaseId();
                int typeId = ids[1];
                int localisationId = ids[2];
                std::string releaseKey = _boundaryStateManager.MakeReleaseKey(contentId, typeId, localisationId);
                if (!_boundaryStateManager.GetStateMachine(releaseKey)) {
                    _boundaryStateManager.StartReleaseStateMachine(contentId, typeId, localisationId);
                }
            });
        });

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

        // Initialiser les state machines pour chaque release du content
        COB_Releases releases = _configurator->GetReleaseRepo()->GetReleases(contentId);
        for (const auto& release : releases) {
            const int* ids = release.GetReleaseId();
            int typeId = ids[1];
            int localisationId = ids[2];
            _boundaryStateManager.StartReleaseStateMachine(contentId, typeId, localisationId);
        }

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

std::string BoundaryManager::GetContentReleasesAsXml(int contentId, int typeId, int localizationId) {
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

std::string BoundaryManager::GetCplsByScriptAsXml(int scriptId) {
    throw std::logic_error("GetCplsByScriptAsXml not implemented");
}


void BoundaryManager::DeleteRelease(int contentId, int typeId, int localisationId) {
    try {
        auto releaseRepo = _configurator->GetReleaseRepo();
        auto release = releaseRepo->GetRelease(contentId, typeId, localisationId);
        // Créer un pointeur pour la suppression
        COB_Release* releasePtr = new COB_Release(release);
        releaseRepo->Remove(releasePtr);
        delete releasePtr;
        
        // Nettoyage de la state machine associée
        std::string releaseKey = _boundaryStateManager.MakeReleaseKey(contentId, typeId, localisationId);
        // La state machine sera nettoyée automatiquement par le BoundaryStateManager
        
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

std::string BoundaryManager::GetServerPairsAsXml() {
    try {
        COB_ServerPairs serverPairs = _configurator->GetServerPairRepo()->GetServerPairs();
        return serverPairs;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get server pairs : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetServerPairsAsXml(int id_serv_pair_config) {
    try {
        COB_ServerPair serverPair = _configurator->GetServerPairRepo()->GetServerPair(id_serv_pair_config);
        return serverPair;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get server pair " + std::to_string(id_serv_pair_config) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetServerPairsBySiteAsXml(int id_site) {
    try {
        COB_ServerPairs serverPairs = _configurator->GetServerPairRepo()->GetServerPairsBySite(id_site);
        return serverPairs;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get server pairs for site " + std::to_string(id_site) + " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}

std::string BoundaryManager::GetReleaseSyncsAsXml(int contentId, int typeId, int localisationId) {
    try {
        COB_Syncs syncs = _configurator->GetSyncRepo()->GetSyncsByRelease(contentId, typeId, localisationId);
        return syncs;
    }
    catch(const std::exception& e) { 
        std::string errorMsg = "Failed to get syncs for release " + std::to_string(contentId) + 
                              "_" + std::to_string(typeId) + "_" + std::to_string(localisationId) + 
                              " : " + std::string(e.what());
        throw std::runtime_error(errorMsg); 
    }
}