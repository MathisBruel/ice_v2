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
            int* type_cpl = result->getIntValue(i, "type_cpl");
            std::string* sha1_sync = result->getStringValue(i, "sha1_sync");
            std::string* path_cpl = result->getStringValue(i, "path_cpl");
            int* id_serv_pair_config = result->getIntValue(i, "id_serv_pair_config");
            int* id_content = result->getIntValue(i, "id_content");
            int* id_type = result->getIntValue(i, "id_type");
            int* id_localisation = result->getIntValue(i, "id_localisation");
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync,
                        type_cpl ? *type_cpl : -1,
                        sha1_sync ? *sha1_sync : "",
                        path_cpl ? *path_cpl : "",
                        id_serv_pair_config ? *id_serv_pair_config : -1,
                        id_content ? *id_content : -1,
                        id_type ? *id_type : -1,
                        id_localisation ? *id_localisation : -1);
            cpls.emplace_back(cpl);
        }
    }
    
    return std::move(cpls);
}

COB_Cpl COB_CplRepo::GetCpl(int id)
{
    std::unique_ptr<ResultQuery> result = _cplRepo->getCpl(id);
    if (!result || !result->isValid() || result->getNbRows() == 0) {
        throw std::runtime_error("Failed to get cpl " + std::to_string(id) + ": " + (result ? result->getErrorMessage() : "null result"));
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
    int* type_cpl = result->getIntValue(0, "type_cpl");
    std::string* sha1_sync = result->getStringValue(0, "sha1_sync");
    std::string* path_cpl = result->getStringValue(0, "path_cpl");
    int* id_serv_pair_config = result->getIntValue(0, "id_serv_pair_config");
    int* id_content = result->getIntValue(0, "id_content");
    int* id_type = result->getIntValue(0, "id_type");
    int* id_localisation = result->getIntValue(0, "id_localisation");
    return COB_Cpl(actualId, *name, *uuid, cplPathSync,
                   type_cpl ? *type_cpl : -1,
                   sha1_sync ? *sha1_sync : "",
                   path_cpl ? *path_cpl : "",
                   id_serv_pair_config ? *id_serv_pair_config : -1,
                   id_content ? *id_content : -1,
                   id_type ? *id_type : -1,
                   id_localisation ? *id_localisation : -1);
}

COB_Cpl COB_CplRepo::GetCplByUuid(const std::string& uuid)
{
    std::unique_ptr<ResultQuery> result = _cplRepo->getCplByUuid(uuid);
    if (!result || !result->isValid() || result->getNbRows() == 0) {
        throw std::runtime_error("Failed to get cpl with uuid " + uuid + ": " + (result ? result->getErrorMessage() : "null result"));
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
    int* type_cpl = result->getIntValue(0, "type_cpl");
    std::string* sha1_sync = result->getStringValue(0, "sha1_sync");
    std::string* path_cpl = result->getStringValue(0, "path_cpl");
    int* id_serv_pair_config = result->getIntValue(0, "id_serv_pair_config");
    int* id_content = result->getIntValue(0, "id_content");
    int* id_type = result->getIntValue(0, "id_type");
    int* id_localisation = result->getIntValue(0, "id_localisation");
    return COB_Cpl(actualId, *name, *cplUuid, cplPathSync,
                   type_cpl ? *type_cpl : -1,
                   sha1_sync ? *sha1_sync : "",
                   path_cpl ? *path_cpl : "",
                   id_serv_pair_config ? *id_serv_pair_config : -1,
                   id_content ? *id_content : -1,
                   id_type ? *id_type : -1,
                   id_localisation ? *id_localisation : -1);
}

COB_Cpls COB_CplRepo::GetCplsByRelease(int releaseId)
{
    COB_Cpls cpls;
    std::unique_ptr<ResultQuery> result = _cplRepo->getCplsByRelease(releaseId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpls for release " + std::to_string(releaseId) + ": " + (result ? result->getErrorMessage() : "null result"));
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
            int* type_cpl = result->getIntValue(i, "type_cpl");
            std::string* sha1_sync = result->getStringValue(i, "sha1_sync");
            std::string* path_cpl = result->getStringValue(i, "path_cpl");
            int* id_serv_pair_config = result->getIntValue(i, "id_serv_pair_config");
            int* id_content = result->getIntValue(i, "id_content");
            int* id_type = result->getIntValue(i, "id_type");
            int* id_localisation = result->getIntValue(i, "id_localisation");
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync,
                        type_cpl ? *type_cpl : -1,
                        sha1_sync ? *sha1_sync : "",
                        path_cpl ? *path_cpl : "",
                        id_serv_pair_config ? *id_serv_pair_config : -1,
                        id_content ? *id_content : -1,
                        id_type ? *id_type : -1,
                        id_localisation ? *id_localisation : -1);
            cpls.emplace_back(cpl);
        }
    }
    return cpls;
}

COB_Cpls COB_CplRepo::GetCplsByRelease(int contentId, int typeId, int localisationId)
{
    COB_Cpls cpls;
    std::unique_ptr<ResultQuery> result = _cplRepo->getCplsByRelease(contentId, typeId, localisationId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpls for release (triple) : " + (result ? result->getErrorMessage() : "null result"));
    }
    int nbRows = result->getNbRows();
    cpls.reserve(nbRows);
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
            int* type_cpl = result->getIntValue(i, "type_cpl");
            std::string* sha1_sync = result->getStringValue(i, "sha1_sync");
            std::string* path_cpl = result->getStringValue(i, "path_cpl");
            int* id_serv_pair_config = result->getIntValue(i, "id_serv_pair_config");
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync,
                        type_cpl ? *type_cpl : -1,
                        sha1_sync ? *sha1_sync : "",
                        path_cpl ? *path_cpl : "",
                        id_serv_pair_config ? *id_serv_pair_config : -1,
                        *id_content,
                        *id_type,
                        *id_localisation);
            cpls.emplace_back(cpl);
        }
    }
    return cpls;
}

COB_Cpls COB_CplRepo::GetCplsBySite(int siteId)
{
    COB_Cpls cpls;
    std::unique_ptr<ResultQuery> result = _cplRepo->getCplsBySite(siteId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get cpls for site " + std::to_string(siteId) + ": " + (result ? result->getErrorMessage() : "null result"));
    }
    int nbRows = result->getNbRows();
    cpls.reserve(nbRows);
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
            int* type_cpl = result->getIntValue(i, "type_cpl");
            std::string* sha1_sync = result->getStringValue(i, "sha1_sync");
            std::string* path_cpl = result->getStringValue(i, "path_cpl");
            int* id_serv_pair_config = result->getIntValue(i, "id_serv_pair_config");
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync,
                        type_cpl ? *type_cpl : -1,
                        sha1_sync ? *sha1_sync : "",
                        path_cpl ? *path_cpl : "",
                        id_serv_pair_config ? *id_serv_pair_config : -1,
                        *id_content,
                        *id_type,
                        *id_localisation);
            cpls.emplace_back(cpl);
        }
    }
    return cpls;
}

COB_Cpls COB_CplRepo::GetUnlinkedCpls()
{
    COB_Cpls cpls;
    std::unique_ptr<ResultQuery> result = _cplRepo->getUnlinkedCpls();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get unlinked cpls: " + (result ? result->getErrorMessage() : "null result"));
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
            int* type_cpl = result->getIntValue(i, "type_cpl");
            std::string* sha1_sync = result->getStringValue(i, "sha1_sync");
            std::string* path_cpl = result->getStringValue(i, "path_cpl");
            int* id_serv_pair_config = result->getIntValue(i, "id_serv_pair_config");
            COB_Cpl cpl(cplId, *name, *uuid, cplPathSync,
                        type_cpl ? *type_cpl : -1,
                        sha1_sync ? *sha1_sync : "",
                        path_cpl ? *path_cpl : "",
                        id_serv_pair_config ? *id_serv_pair_config : -1);
            cpls.emplace_back(cpl);
        }
    }
    return cpls;
}

void COB_CplRepo::Create(COB_Cpl* cpl) {
    _cplRepo->Create(cpl);
}

void COB_CplRepo::Remove(COB_Cpl* cpl) {
    _cplRepo->Remove(cpl);
}

void COB_CplRepo::Update(COB_Cpl* cpl) {
    _cplRepo->Update(cpl);
}

Query* COB_CplRepo::GetQuery() const {
    return static_cast<MySQLCplRepo*>(_cplRepo.get())->GetQuery();
}
