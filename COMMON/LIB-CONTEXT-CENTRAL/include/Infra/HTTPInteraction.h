#pragma once
class HTTPInteraction
{
public:
    virtual void Run() = 0;
    virtual void SetDatas(std::string cmdUUID, std::map<std::string, std::string> Params) {
        this->UUID = cmdUUID;
    };
    std::string GetUUID() { return this->UUID; }
    std::string GetStatus() { return this->response.cmdStatus; }
    std::string GetComments() { return this->response.cmdComment; }
    std::string GetDatasXML() { return this->response.cmdDatasXML; }
    virtual int GetContentId() { return -1; }
protected:
    std::string UUID;
    TransitionResponse response;
};