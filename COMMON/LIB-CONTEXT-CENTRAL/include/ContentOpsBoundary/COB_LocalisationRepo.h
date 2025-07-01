#pragma once
#include <memory>
#include <vector>
#include "ContentOpsDomain/COD_LocalisationRepo.h"
#include "ContentOpsBoundary/COB_Localisation.h"
#include "ContentOpsBoundary/COB_Localisations.h"

class COB_LocalisationRepo
{
public:
    COB_LocalisationRepo(std::shared_ptr<COD_LocalisationRepo> localisationRepo);
    ~COB_LocalisationRepo();

    COB_Localisations GetLocalisations();
    COB_Localisation GetLocalisation(int id);

private:
    std::shared_ptr<COD_LocalisationRepo> _localisationRepo;
}; 