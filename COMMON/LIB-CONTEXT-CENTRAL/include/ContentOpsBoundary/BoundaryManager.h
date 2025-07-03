#pragma once
#include "BoundaryEnum.h"
#include "BoundaryStateMachine.h"
#include "BoundaryStateManager.h"
#include <stdexcept>
#include <memory>
#include "ContentOpsBoundary/COB_SiteRepo.h"
#include "ContentOpsBoundary/COB_GroupRepo.h"
#include "ContentOpsBoundary/COB_ContentRepo.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsBoundary/COB_LocalisationRepo.h"
#include "ContentOpsBoundary/COB_TypeRepo.h"
#include "ContentOpsBoundary/COB_CplRepo.h"
#include "ContentOpsBoundary/COB_Content.h"

using namespace ContentOpsBoundaryEnum;

class BoundaryManager {
    public:
    BoundaryManager();
    ~BoundaryManager();
    void AddStateMachine(int id, BoundaryStateMachine* stateMachine);
    BoundaryStateMachine* GetStateMachine(int id);
    BoundaryStateMachine* CreateStateMachine(int contentId, MySQLDBConnection* dbConnection);
    COB_Content* CreateContent(std::string title);
    std::string GetAllContentsAsXml();
    std::string GetContentAsXml(int contentId);
    std::string GetContentReleasesAsXml(int contentId);
    std::string GetContentReleasesAsXml(int contentId,int typeId, int localizationId);
    std::string GetGroupsAsXml();
    std::string GetGroupAsXml(int groupeId);
    std::string GetSitesAsXml(int groupId);
    std::string GetSiteCplsAsXml(int siteId);
    std::string GetLocalisationsAsXml();
    std::string GetTypesAsXml();
    std::string GetCplsAsXml();
    std::string GetCplAsXml(int id);
    std::string GetCplAsXmlByUuid(const std::string& uuid);
    std::string GetCplsByScriptAsXml(int scriptId);
    std::string GetCplsByReleaseAsXml(int releaseId);
    std::string GetReleaseCplsAsXml(int contentId, int typeId, int localisationId);
    std::string GetUnlinkedCplsAsXml();
    void UpdateContent(int contentId);
    BoundaryStateManager GetBoundaryStateManager() {return _boundaryStateManager;}

    static BoundaryManager& GetInstance() {
        static BoundaryManager instance;
        return instance;
    }

    BoundaryManager(const BoundaryManager&) = delete;
    BoundaryManager& operator=(const BoundaryManager&) = delete;

    private:
    BoundaryStateManager _boundaryStateManager;
    std::shared_ptr<COB_SiteRepo> _siteRepo;
    std::shared_ptr<COB_GroupRepo> _groupRepo;
    std::shared_ptr<COB_ContentRepo> _contentRepo;
    std::shared_ptr<COB_ReleaseRepo> _releaseRepo;
    std::shared_ptr<COB_LocalisationRepo> _localisationRepo;
    std::shared_ptr<COB_TypeRepo> _typeRepo;
    std::shared_ptr<COB_CplRepo> _cplRepo;
};