#pragma once
#include "BoundaryEnum.h"
#include "BoundaryStateMachine.h"
#include "BoundaryStateManager.h"
#include "COB_Configurator.h"
#include "ContentOpsApp/TransitionResponse.h"
#include <stdexcept>
#include <memory>

using namespace ContentOpsBoundaryEnum;

class BoundaryManager {
public:
    BoundaryManager();
    ~BoundaryManager();
    TransitionResponse CreateContent(std::string title);
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
    std::shared_ptr<COB_Configurator> GetConfigurator() {return _configurator;}

    static BoundaryManager& GetInstance() {
        static BoundaryManager instance;
        return instance;
    }

    BoundaryManager(const BoundaryManager&) = delete;
    BoundaryManager& operator=(const BoundaryManager&) = delete;

private:
    BoundaryStateManager _boundaryStateManager;
    std::shared_ptr<COB_Configurator> _configurator;
};