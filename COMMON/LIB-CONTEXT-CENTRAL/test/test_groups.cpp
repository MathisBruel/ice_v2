#include <iostream>
#include "ContentOpsBoundary/BoundaryManager.h"
#include "ContentOpsBoundary/COB_Group.h"
#include "ContentOpsInfra/MySQLGroupRepo.h"

int main() {
    try {
        // Test de création d'un groupe
        COB_Group group;
        group.SetGroupId(1);
        group.SetDatas("Europe", -1);
        
        std::cout << "Group XML: " << group.toXmlString() << std::endl;
        
        // Test avec un groupe parent
        COB_Group groupChild;
        groupChild.SetGroupId(6);
        groupChild.SetDatas("France", 1);
        
        std::cout << "Child Group XML: " << groupChild.toXmlString() << std::endl;
        
        // Test du BoundaryManager
        BoundaryManager& manager = BoundaryManager::GetInstance();
        
        // Note: Ces appels nécessiteraient une vraie connexion DB
        // std::string groupsXml = manager.GetGroupsAsXml();
        // std::cout << "All Groups: " << groupsXml << std::endl;
        
        // Test de récupération d'un groupe spécifique (optimisé avec requête SQL directe)
        // std::string groupXml = manager.GetGroupAsXml(1);
        // std::cout << "Group 1: " << groupXml << std::endl;
        
        std::cout << "Tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 