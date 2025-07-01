#pragma once
#include <memory>
#include <vector>
#include "ResultQuery.h"

class COD_Cpl;

class COD_CplRepo
{
public:
    virtual ~COD_CplRepo() = default;
    
    // Méthodes virtuelles pures pour les opérations CRUD
    virtual std::unique_ptr<ResultQuery> getCpls() = 0;
    virtual std::unique_ptr<ResultQuery> getCpl(int id) = 0;
    virtual std::unique_ptr<ResultQuery> getCplByUuid(const std::string& uuid) = 0;
    virtual std::unique_ptr<ResultQuery> getCplsByScript(int scriptId) = 0;
    virtual std::unique_ptr<ResultQuery> getCplsByRelease(int releaseId) = 0;
    virtual std::unique_ptr<ResultQuery> getCplsByRelease(int contentId, int typeId, int localisationId) = 0;
    virtual std::unique_ptr<ResultQuery> getUnlinkedCpls() = 0;
    virtual std::unique_ptr<ResultQuery> getCplsBySite(int siteId) = 0;
}; 