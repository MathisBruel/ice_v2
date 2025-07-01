#include "ContentOpsBoundary/COB_LocalisationRepo.h"
#include "ContentOpsDomain/COD_LocalisationRepo.h"
#include "ContentOpsBoundary/COB_Localisations.h"

COB_LocalisationRepo::COB_LocalisationRepo(std::shared_ptr<COD_LocalisationRepo> localisationRepo)
{
    _localisationRepo = localisationRepo;
}

COB_LocalisationRepo::~COB_LocalisationRepo()
{
}

COB_Localisations COB_LocalisationRepo::GetLocalisations()
{
    COB_Localisations localisations;
    std::unique_ptr<ResultQuery> result = _localisationRepo->getLocalisations();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get localisations : " + std::string(result->getErrorMessage())); 
    }
    int nbRows = result->getNbRows();
    localisations.reserve(nbRows);
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_localisation");
        std::string* name = result->getStringValue(i, "name");
        if (id && name) {
            COB_Localisation localisation(*id, *name);
            localisations.emplace_back(localisation);
        }
    }
    return std::move(localisations);
}

COB_Localisation COB_LocalisationRepo::GetLocalisation(int id)
{
    std::unique_ptr<ResultQuery> result = _localisationRepo->getLocalisation(id);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get localisation " + std::to_string(id) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    if (result->getNbRows() == 0) {
        throw std::runtime_error("Localisation not found with id " + std::to_string(id)); 
    }
    int* localisationId = result->getIntValue(0, "id_localisation");
    std::string* name = result->getStringValue(0, "name");
    
    COB_Localisation localisation(*localisationId, *name);
    return std::move(localisation);
} 