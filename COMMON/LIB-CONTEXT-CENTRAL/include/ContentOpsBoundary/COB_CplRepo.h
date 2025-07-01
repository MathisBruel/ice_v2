#pragma once
#include <memory>
#include <vector>
#include <string>
#include "ContentOpsDomain/COD_CplRepo.h"
#include "ContentOpsBoundary/COB_Cpl.h"
#include "ContentOpsBoundary/COB_Cpls.h"

class COB_CplRepo
{
public:
    COB_CplRepo(std::shared_ptr<COD_CplRepo> cplRepo);
    ~COB_CplRepo();

    COB_Cpls GetCpls();
    COB_Cpl GetCpl(int id);
    COB_Cpl GetCplByUuid(const std::string& uuid);
    COB_Cpls GetCplsByScript(int scriptId);
    COB_Cpls GetCplsByRelease(int releaseId);
    COB_Cpls GetCplsByRelease(int contentId, int typeId, int localisationId);
    COB_Cpls GetUnlinkedCpls();
    COB_Cpls GetCplsBySite(int siteId);

private:
    std::shared_ptr<COD_CplRepo> _cplRepo;
}; 