#pragma once
#include "COB_Interaction.h"
#include "ContentOpsApp/TransitionResponse.h"
#include <functional>
#include <map>
#include <string>

class COB_UploadInteraction : public COB_Interaction {
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateUploadCIS;
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateUploadSyncLoop;
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfFinishSyncLoop;
    
    COB_UploadInteraction() 
        : pfStateUploadCIS([](std::string UUID, std::map<std::string, std::string> Params) {
              TransitionResponse response;
              response.cmdUUID = UUID;
              response.cmdStatus = "KO";
              response.cmdComment = "StateUploadCIS function not configured";
              return response;
          }),
          pfStateUploadSyncLoop([](std::string UUID, std::map<std::string, std::string> Params) {
              TransitionResponse response;
              response.cmdUUID = UUID;
              response.cmdStatus = "KO";
              response.cmdComment = "StateUploadSyncLoop function not configured";
              return response;
          }),
          pfFinishSyncLoop([](std::string UUID, std::map<std::string, std::string> Params) {
              TransitionResponse response;
              response.cmdUUID = UUID;
              response.cmdStatus = "KO";
              response.cmdComment = "FinishSyncLoop function not configured";
              return response;
          })
    {
    }
    
    virtual TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) override {
        return pfStateUploadCIS(cmdUUID, Params);
    }
}; 