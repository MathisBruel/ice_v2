#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsBoundary/COB_Releases.h"
#include "ContentOpsDomain/COD_ReleaseRepo.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"

COB_ReleaseRepo::COB_ReleaseRepo(std::shared_ptr<COD_ReleaseRepo> releaseRepo)
{
    _releaseRepo = releaseRepo;
}

COB_ReleaseRepo::~COB_ReleaseRepo()
{
}

COB_Releases COB_ReleaseRepo::GetReleases()
{
    COB_Releases releases;
    std::unique_ptr<ResultQuery> result = _releaseRepo->getReleases();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get releases : " + std::string(result->getErrorMessage())); 
    }
    int nbRows = result->getNbRows();
    releases.reserve(nbRows);
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_content");
        int* type = result->getIntValue(i, "id_type");
        int* localisation = result->getIntValue(i, "id_localisation");
        std::string* cplRefPath = result->getStringValue(i, "release_cpl_ref_path");
        std::string* cisPath = result->getStringValue(i, "release_cis_path");
        std::string* typeName = result->getStringValue(i, "type_name");
        std::string* localisationName = result->getStringValue(i, "localisation_name");
        if (id && type && localisation && cplRefPath) {
            COB_Release release(*id, *type, *localisation);
            release.SetReleaseInfos(*cplRefPath);
            if (typeName) {
                release.SetTypeName(*typeName);
            }
            if (localisationName) {
                release.SetLocalisationName(*localisationName);
            }
            releases.emplace_back(release);
        }
    }
    return std::move(releases);
}

COB_Releases COB_ReleaseRepo::GetReleases(int contentId)
{
    COB_Releases releases;
    std::unique_ptr<ResultQuery> result = _releaseRepo->getReleases(contentId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get releases for content " + std::to_string(contentId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    int nbRows = result->getNbRows();
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_content");
        int* type = result->getIntValue(i, "id_type");
        int* localisation = result->getIntValue(i, "id_localisation");
        std::string* cplRefPath = result->getStringValue(i, "release_cpl_ref_path");
        std::string* cisPath = result->getStringValue(i, "release_cis_path");
        std::string* typeName = result->getStringValue(i, "type_name");
        std::string* localisationName = result->getStringValue(i, "localisation_name");
        if (id && type && localisation && cplRefPath) {
            COB_Release release(*id, *type, *localisation);
            release.SetReleaseInfos(*cplRefPath);
            if (typeName) {
                release.SetTypeName(*typeName);
            }
            if (localisationName) {
                release.SetLocalisationName(*localisationName);
            }
            releases.emplace_back(release);
        }
    }
    return std::move(releases);
}

COB_Release COB_ReleaseRepo::GetRelease(int contentId, int typeId, int localisationId)
{
    std::unique_ptr<ResultQuery> result = _releaseRepo->getRelease(contentId, typeId, localisationId);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get release for content " + std::to_string(contentId) + " with type " + std::to_string(typeId) + " and localisation " + std::to_string(localisationId) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    if (result->getNbRows() == 0) {
        throw std::runtime_error("Release not found for content " + std::to_string(contentId) + " with type " + std::to_string(typeId) + " and localisation " + std::to_string(localisationId)); 
    }
    int* id = result->getIntValue(0, "id_content");
    int* type = result->getIntValue(0, "id_type");
    int* localisation = result->getIntValue(0, "id_localisation");
    std::string* cplRefPath = result->getStringValue(0, "release_cpl_ref_path");
    std::string* cisPath = result->getStringValue(0, "release_cis_path");
    std::string* typeName = result->getStringValue(0, "type_name");
    std::string* localisationName = result->getStringValue(0, "localisation_name");
    
    COB_Release release(*id, *type, *localisation);
    release.SetReleaseInfos(*cplRefPath);
    if (typeName) {
        release.SetTypeName(*typeName);
    }
    if (localisationName) {
        release.SetLocalisationName(*localisationName);
    }
    return std::move(release);
}
