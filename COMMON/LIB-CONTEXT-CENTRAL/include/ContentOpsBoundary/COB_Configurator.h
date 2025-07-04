#pragma once
#include "COB_Context.h"
#include <map>
#include "commandCentral.h"
#include "ContentOpsBoundary/COB_SiteRepo.h"
#include "ContentOpsBoundary/COB_GroupRepo.h"
#include "ContentOpsBoundary/COB_ContentRepo.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsBoundary/COB_LocalisationRepo.h"
#include "ContentOpsBoundary/COB_TypeRepo.h"
#include "ContentOpsBoundary/COB_CplRepo.h"
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include "ContentOpsInfra/MySQLSiteRepo.h"
#include "ContentOpsInfra/MySQLGroupRepo.h"
#include "ContentOpsInfra/MySQLContentRepo.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"
#include "ContentOpsInfra/MySQLLocalisationRepo.h"
#include "ContentOpsInfra/MySQLTypeRepo.h"
#include "ContentOpsInfra/MySQLCplRepo.h"


class COB_Configurator {
public:
    COB_Configurator();
    ~COB_Configurator() = default;

    std::shared_ptr<MySQLDBConnection> GetDBConnection() { return _dbConn; }

    std::shared_ptr<COB_SiteRepo> GetSiteRepo() { return _siteRepo; }
    std::shared_ptr<COB_GroupRepo> GetGroupRepo() { return _groupRepo; }
    std::shared_ptr<COB_ContentRepo> GetContentRepo() { return _contentRepo; }
    std::shared_ptr<COB_ReleaseRepo> GetReleaseRepo() { return _releaseRepo; }
    std::shared_ptr<COB_LocalisationRepo> GetLocalisationRepo() { return _localisationRepo; }
    std::shared_ptr<COB_TypeRepo> GetTypeRepo() { return _typeRepo; }
    std::shared_ptr<COB_CplRepo> GetCplRepo() { return _cplRepo; }


private:
    std::shared_ptr<MySQLDBConnection> _dbConn;
    std::shared_ptr<COB_SiteRepo> _siteRepo;
    std::shared_ptr<COB_GroupRepo> _groupRepo;
    std::shared_ptr<COB_ContentRepo> _contentRepo;
    std::shared_ptr<COB_ReleaseRepo> _releaseRepo;
    std::shared_ptr<COB_LocalisationRepo> _localisationRepo;
    std::shared_ptr<COB_TypeRepo> _typeRepo;
    std::shared_ptr<COB_CplRepo> _cplRepo;
}; 