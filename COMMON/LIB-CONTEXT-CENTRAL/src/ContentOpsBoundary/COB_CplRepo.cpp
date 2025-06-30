#include "ContentOpsBoundary/COB_CplRepo.h"
#include "ContentOpsBoundary/COB_Cpl.h"
#include "ContentOpsBoundary/COB_Cpls.h"
#include "ContentOpsDomain/COD_CplRepo.h"

COB_CplRepo::COB_CplRepo(std::shared_ptr<COD_CplRepo> cplRepo)
{
    _cplRepo = cplRepo;
}

COB_CplRepo::~COB_CplRepo()
{
}

COB_Cpls COB_CplRepo::GetCpls()
{
    COB_Cpls cpls;
    std::unique_ptr<ResultQuery> result = _cplRepo->getCpls();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpls : " + std::string(result ? result->getErrorMessage() : "null result")); 
    }
    
    int nbRows = result->getNbRows();
    cpls.reserve(nbRows);
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id");
        std::string* name = result->getStringValue(i, "name");
        std::string* uuid = result->getStringValue(i, "uuid");
        std::string* pathSync = result->getStringValue(i, "path_sync");
        
        if (name && uuid) {
            int cplId = id ? *id : -1;
            std::string cplPathSync = pathSync ? *pathSync : "";
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync);
            cpls.emplace_back(cpl);
        }
    }
    
    return std::move(cpls);
}

std::string COB_CplRepo::GetCplsAsXml()
{
    std::string xml = "<cpls>";
    std::unique_ptr<ResultQuery> result = _cplRepo->getCpls();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpls: " + std::string(result->getErrorMessage())); 
    }
    
    int nbRows = result->getNbRows();
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id");
        std::string* name = result->getStringValue(i, "name");
        std::string* uuid = result->getStringValue(i, "uuid");
        std::string* pathSync = result->getStringValue(i, "path_sync");
        if (name && uuid) {
            int cplId = id ? *id : -1;
            std::string cplPathSync = pathSync ? *pathSync : "";
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync);
            xml += static_cast<std::string>(cpl);
        }
    }
    xml += "</cpls>";
    return xml;
}

std::string COB_CplRepo::GetCplAsXml(int id)
{
    std::unique_ptr<ResultQuery> result = _cplRepo->getCpl(id);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpl " + std::to_string(id) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    if (result->getNbRows() == 0) {
        throw std::runtime_error("Cpl not found with id " + std::to_string(id)); 
    }
    
    int* cplId = result->getIntValue(0, "id");
    std::string* name = result->getStringValue(0, "name");
    std::string* uuid = result->getStringValue(0, "uuid");
    std::string* pathSync = result->getStringValue(0, "path_sync");
    
    if (!name || !uuid) {
        throw std::runtime_error("Failed to get cpl data from result");
    }
    
    int actualId = cplId ? *cplId : -1;
    std::string cplPathSync = pathSync ? *pathSync : "";
    COB_Cpl cpl(actualId, *name, *uuid, cplPathSync);
    return static_cast<std::string>(cpl);
}

std::string COB_CplRepo::GetCplAsXmlByUuid(const std::string& uuid)
{
    std::unique_ptr<ResultQuery> result = _cplRepo->getCplByUuid(uuid);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpl with uuid " + uuid + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    if (result->getNbRows() == 0) {
        throw std::runtime_error("Cpl not found with uuid " + uuid); 
    }
    
    int* cplId = result->getIntValue(0, "id");
    std::string* name = result->getStringValue(0, "name");
    std::string* cplUuid = result->getStringValue(0, "uuid");
    std::string* pathSync = result->getStringValue(0, "path_sync");
    
    if (!name || !cplUuid) {
        throw std::runtime_error("Failed to get cpl data from result");
    }
    
    int actualId = cplId ? *cplId : -1;
    std::string cplPathSync = pathSync ? *pathSync : "";
    COB_Cpl cpl(actualId, *name, *cplUuid, cplPathSync);
    return static_cast<std::string>(cpl);
}

std::string COB_CplRepo::GetCplsByScriptAsXml(int scriptId)
{
    std::string xml = "<cpls>";
    std::unique_ptr<ResultQuery> result = _cplRepo->getCplsByScript(scriptId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpls for script " + std::to_string(scriptId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    
    int nbRows = result->getNbRows();
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id");
        std::string* name = result->getStringValue(i, "name");
        std::string* uuid = result->getStringValue(i, "uuid");
        std::string* pathSync = result->getStringValue(i, "path_sync");
        if (name && uuid) {
            int cplId = id ? *id : -1;
            std::string cplPathSync = pathSync ? *pathSync : "";
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync);
            xml += static_cast<std::string>(cpl);
        }
    }
    xml += "</cpls>";
    return xml;
}

std::string COB_CplRepo::GetCplsByReleaseAsXml(int releaseId)
{
    std::string xml = "<cpls>";
    std::unique_ptr<ResultQuery> result = _cplRepo->getCplsByRelease(releaseId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpls for release " + std::to_string(releaseId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    
    int nbRows = result->getNbRows();
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id");
        std::string* name = result->getStringValue(i, "name");
        std::string* uuid = result->getStringValue(i, "uuid");
        std::string* pathSync = result->getStringValue(i, "path_sync");
        if (name && uuid) {
            int cplId = id ? *id : -1;
            std::string cplPathSync = pathSync ? *pathSync : "";
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync);
            xml += static_cast<std::string>(cpl);
        }
    }
    xml += "</cpls>";
    return xml;
}

std::string COB_CplRepo::GetUnlinkedCplsAsXml()
{
    std::string xml = "<cpls>";
    std::unique_ptr<ResultQuery> result = _cplRepo->getUnlinkedCpls();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get unlinked cpls: " + (result ? result->getErrorMessage() : "null result")); 
    }
    
    int nbRows = result->getNbRows();
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id");
        std::string* name = result->getStringValue(i, "name");
        std::string* uuid = result->getStringValue(i, "uuid");
        std::string* pathSync = result->getStringValue(i, "path_sync");
        
        if (name && uuid) {
            int cplId = id ? *id : -1;
            std::string cplPathSync = pathSync ? *pathSync : "";
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync);
            xml += static_cast<std::string>(cpl);
        }
    }
    
    xml += "</cpls>";
    return xml;
}

std::string COB_CplRepo::GetCplsBySiteAsXml(int siteId)
{
    std::string xml = "<cpls>";
    std::unique_ptr<ResultQuery> result = _cplRepo->getCplsBySite(siteId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpls for site " + std::to_string(siteId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    int nbRows = result->getNbRows();
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id");
        std::string* name = result->getStringValue(i, "name");
        std::string* uuid = result->getStringValue(i, "uuid");
        std::string* pathSync = result->getStringValue(i, "path_sync");
        int* id_content = result->getIntValue(i, "id_content");
        int* id_type = result->getIntValue(i, "id_type");
        int* id_localisation = result->getIntValue(i, "id_localisation");
        if (name && uuid && id_content && id_type && id_localisation) {
            int cplId = id ? *id : -1;
            std::string cplPathSync = pathSync ? *pathSync : "";
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync, *id_content, *id_type, *id_localisation);
            xml += static_cast<std::string>(cpl);
        }
    }
    xml += "</cpls>";
    return xml;
} 