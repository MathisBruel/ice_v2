#include "ContentOpsBoundary/COB_Configurator.h"

COB_Configurator::COB_Configurator() {
    _dbConn = std::make_shared<MySQLDBConnection>();
    _dbConn->InitConnection();
    _siteRepo = std::make_shared<COB_SiteRepo>(std::make_shared<MySQLSiteRepo>());
    _groupRepo = std::make_shared<COB_GroupRepo>(std::make_shared<MySQLGroupRepo>(_dbConn));
    _contentRepo = std::make_shared<COB_ContentRepo>(std::make_shared<MySQLContentRepo>());
    _releaseRepo = std::make_shared<COB_ReleaseRepo>(std::make_shared<MySQLReleaseRepo>());
    _localisationRepo = std::make_shared<COB_LocalisationRepo>(std::make_shared<MySQLLocalisationRepo>());
    _typeRepo = std::make_shared<COB_TypeRepo>(std::make_shared<MySQLTypeRepo>());
    _cplRepo = std::make_shared<COB_CplRepo>(std::make_shared<MySQLCplRepo>());
    _serverPairRepo = std::make_shared<COB_ServerPairRepo>(std::make_shared<MySQLServerPairRepo>(_dbConn.get()));
    _syncRepo = std::make_shared<COB_SyncRepo>(std::make_shared<MySQLSyncRepo>());
    _syncLoopRepo = std::make_shared<COB_SyncLoopRepo>(std::make_shared<MySQLSyncLoopRepo>());
} 