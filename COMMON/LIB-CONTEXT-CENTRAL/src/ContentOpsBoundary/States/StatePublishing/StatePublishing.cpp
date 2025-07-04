#include "ContentOpsBoundary/States/StatePublishing/StatePublishing.h"
#include "ContentOpsBoundary/BoundaryManager.h"

// Implémentation des fonctions de gestion des suppressions
void handleDeleteRelease(const std::map<std::string, std::string>& Params) {
    BoundaryManager::GetInstance().DeleteRelease(
        std::stoi(Params.at("id_movie")), 
        std::stoi(Params.at("id_type")), 
        std::stoi(Params.at("id_localisation"))
    );
}

void handleDeleteCPL(const std::map<std::string, std::string>& Params) {
    BoundaryManager::GetInstance().DeleteCPL(
        std::stoi(Params.at("id_movie")), 
        std::stoi(Params.at("id_type")), 
        std::stoi(Params.at("id_localisation")),
        std::stoi(Params.at("id_serv_pair_config"))
    );
}

void handleDeleteSync(const std::map<std::string, std::string>& Params) {
    BoundaryManager::GetInstance().DeleteSync(
        std::stoi(Params.at("id_movie")), 
        std::stoi(Params.at("id_type")), 
        std::stoi(Params.at("id_localisation")),
        std::stoi(Params.at("id_serv_pair_config"))
    );
}

void handleDeleteSyncLoop(const std::map<std::string, std::string>& Params) {
    BoundaryManager::GetInstance().DeleteSyncLoop(
        std::stoi(Params.at("id_movie")), 
        std::stoi(Params.at("id_type")), 
        std::stoi(Params.at("id_localisation")),
        std::stoi(Params.at("id_serv_pair_config"))
    );
} 