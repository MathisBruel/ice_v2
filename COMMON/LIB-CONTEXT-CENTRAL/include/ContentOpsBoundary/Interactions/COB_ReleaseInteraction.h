#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/ReleaseInteraction.h"
#include <functional>

class COB_ReleaseInteraction : public COB_Interaction, public ReleaseInteraction {
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateCreateRelease;
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateWaitCloseRelease;
    
    COB_ReleaseInteraction() 
        : pfStateCreateRelease([](std::string UUID, std::map<std::string, std::string> Params) {
              TransitionResponse response;
              response.cmdUUID = UUID;
              response.cmdStatus = "KO";
              response.cmdComment = "StateCreateRelease function not configured";
              return response;
          }),
          pfStateWaitCloseRelease([](std::string UUID, std::map<std::string, std::string> Params) {
              TransitionResponse response;
              response.cmdUUID = UUID;
              response.cmdStatus = "KO";
              response.cmdComment = "StateWaitCloseRelease function not configured";
              return response;
          })
    {
    }
    
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateCreateRelease(cmdUUID, Params);
    }
    void Run() { pfTransitionToPublishing(); }
}; 