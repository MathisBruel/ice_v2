#pragma once
#include "App/TransitionResponse.h"

class HTTPInteraction {
public:
    virtual TransitionResponse Run(std::string cmdUUID, std::map<std::string, std::string> Params) { return TransitionResponse(); };
    // virtual void SetDatas(std::string cmdUUID, std::map<std::string, std::string> Params) {
    //     this->response->cmdUUID = cmdUUID;
    // };
    // std::string GetUUID() { return this->response->cmdUUID; }
    // std::string GetStatus() { return this->response->cmdStatus; }
    // std::string GetComments() { return this->response->cmdComment; }
    // std::string GetDatasXML() { return this->response->cmdDatasXML; }
    // HTTPInteraction() { this->response = new TransitionResponse(); }
};