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

    // Méthodes pour récupérer les CPLs
    COB_Cpls GetCpls();
    std::string GetCplsAsXml();
    std::string GetCplAsXml(int id);
    std::string GetCplAsXmlByUuid(const std::string& uuid);
    std::string GetCplsByScriptAsXml(int scriptId);
    std::string GetCplsByReleaseAsXml(int releaseId);
    std::string GetUnlinkedCplsAsXml();
    std::string GetCplsBySiteAsXml(int siteId);

private:
    std::shared_ptr<COD_CplRepo> _cplRepo;
}; 