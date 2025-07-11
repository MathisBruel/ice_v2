#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/CPLInteraction.h"
#include <functional>

class COB_CPLInteraction : public COB_Interaction, public CPLInteraction {
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateGotNewCPL;
    
    COB_CPLInteraction() 
        : pfStateGotNewCPL([](std::string UUID, std::map<std::string, std::string> Params) {
              TransitionResponse response;
              response.cmdUUID = UUID;
              response.cmdStatus = "KO";
              response.cmdComment = "StateGotNewCPL function not configured";
              return response;
          })
    {
    }
    
    TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) {
        return pfStateCPL(cmdUUID, Params);
    }
    void Run() { pfTransitionToSync(); }
}; 