#include "ContentOpsBoundary/COB_TypeRepo.h"
#include "ContentOpsDomain/COD_TypeRepo.h"
#include "ContentOpsBoundary/COB_Types.h"

COB_TypeRepo::COB_TypeRepo(std::shared_ptr<COD_TypeRepo> typeRepo)
{
    _typeRepo = typeRepo;
}

COB_TypeRepo::~COB_TypeRepo()
{
}

COB_Types COB_TypeRepo::GetTypes()
{
    COB_Types types;
    std::unique_ptr<ResultQuery> result = _typeRepo->getTypes();
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get types : " + std::string(result->getErrorMessage())); 
    }
    int nbRows = result->getNbRows();
    types.reserve(nbRows);
    for (int i = 0; i < nbRows; ++i) {
        int* id = result->getIntValue(i, "id_type");
        std::string* name = result->getStringValue(i, "name");
        if (id && name) {
            COB_Type type(*id, *name);
            types.emplace_back(type);
        }
    }
    return std::move(types);
}

COB_Type COB_TypeRepo::GetType(int id)
{
    std::unique_ptr<ResultQuery> result = _typeRepo->getType(id);
    if (!result || !result->isValid()) {
        throw std::runtime_error("Failed to get type " + std::to_string(id) + ": " + (result ? result->getErrorMessage() : "null result")); 
    }
    if (result->getNbRows() == 0) {
        throw std::runtime_error("Type not found with id " + std::to_string(id)); 
    }
    int* typeId = result->getIntValue(0, "id_type");
    std::string* name = result->getStringValue(0, "name");
    
    COB_Type type(*typeId, *name);
    return std::move(type);
} 