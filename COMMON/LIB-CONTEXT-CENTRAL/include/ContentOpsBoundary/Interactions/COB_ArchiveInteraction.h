#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/ReleaseInteraction.h"
#include <functional>

class COB_ArchiveInteraction : public COB_Interaction, public ReleaseInteraction {
public:
    std::function<void(std::string, std::map<std::string, std::string>)> pfStateArchiveRelease;
    
    COB_ArchiveInteraction() 
        : pfStateArchiveRelease([](std::string UUID, std::map<std::string, std::string> Params) {
          })
    {
    }
    
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        TransitionResponse response;
        response.cmdUUID = cmdUUID;
        response.cmdStatus = "OK";
        response.cmdComment = "Archive process completed";
        return response;
    }
    void Run() { pfTransitionToPublishing(); }
}; 