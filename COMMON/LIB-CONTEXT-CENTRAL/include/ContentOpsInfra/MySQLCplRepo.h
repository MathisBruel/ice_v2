#pragma once
#include <iostream>
#include "ContentOpsDomain/COD_CplRepo.h"
#include "Query.h"

class MySQLCplRepo : public COD_CplRepo
{
public:
    // Implémentation des méthodes virtuelles pures
    std::unique_ptr<ResultQuery> getCpls() override;
    std::unique_ptr<ResultQuery> getCpl(int id) override;
    std::unique_ptr<ResultQuery> getCplByUuid(const std::string& uuid) override;
    std::unique_ptr<ResultQuery> getCplsByScript(int scriptId) override;
    std::unique_ptr<ResultQuery> getCplsByRelease(int releaseId) override;
    std::unique_ptr<ResultQuery> getCplsByRelease(int contentId, int typeId, int localisationId) override;
    std::unique_ptr<ResultQuery> getUnlinkedCpls() override;
    std::unique_ptr<ResultQuery> getCplsBySite(int siteId);

private:
    static std::string _database;
    static std::string _table;
}; 