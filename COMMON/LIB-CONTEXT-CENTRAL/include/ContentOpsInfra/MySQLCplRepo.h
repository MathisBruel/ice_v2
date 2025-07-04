#pragma once
#include <iostream>
#include "ContentOpsDomain/COD_CplRepo.h"
#include "Query.h"
#include "ResultQuery.h"
#include "ContentOpsDomain/COD_Cpl.h"
#include "ContentOpsBoundary/COB_Cpl.h"
#include "ContentOpsInfra/MySQLDBConnection.h"
#include <memory>

class MySQLCplRepo : public COD_CplRepo
{
public:
    MySQLCplRepo();
    virtual ~MySQLCplRepo();

    void Create(COB_Cpl* cpl) override;
    void Remove(COB_Cpl* cpl) override;
    void Update(COB_Cpl* cpl) override;

    std::unique_ptr<Query> MySQLcreate(COB_Cpl* cpl);
    std::unique_ptr<Query> MySQLread(COB_Cpl* cpl);
    std::unique_ptr<Query> MySQLread();
    std::unique_ptr<Query> MySQLupdate(COB_Cpl* cpl);
    std::unique_ptr<Query> MySQLremove(COB_Cpl* cpl);

    Query* GetQuery() { return _query.get(); }

    std::unique_ptr<ResultQuery> getCpls() override;
    std::unique_ptr<ResultQuery> getCpl(int id) override;
    std::unique_ptr<ResultQuery> getCplByUuid(const std::string& uuid) override;
    std::unique_ptr<ResultQuery> getCplsByScript(int scriptId) override;
    std::unique_ptr<ResultQuery> getCplsByRelease(int releaseId) override;
    std::unique_ptr<ResultQuery> getCplsByRelease(int contentId, int typeId, int localisationId) override;
    std::unique_ptr<ResultQuery> getUnlinkedCpls() override;
    std::unique_ptr<ResultQuery> getCplsBySite(int siteId) override;

private:
    static std::string _database;
    static std::string _table;

    int* _cplIds;
    std::string _cplName;
    std::unique_ptr<Query> _query;
}; 