#pragma once
#include "HTTPInteraction.h"
#include "App/ContentInteraction.h"

class HTTPContentInteraction : public HTTPInteraction , public ContentInteraction
{
public:
    void Run() override { 
        try {
        this->response = ContentInteraction::pfTransitionToPublishing(this->UUID, this->_contentTitle);
        }
        catch (std::exception& e) {
            std::cout << "Exception: " << e.what() << std::endl;
        }
    }
    void SetDatas(std::string cmdUUID, std::map<std::string, std::string> Params) override { 
        this->UUID = cmdUUID;
        this->_contentTitle = Params["contentTitle"]; 
    }
    std::string GetUUID() { return this->response.cmdUUID; }
    std::string GetStatus() { return this->response.cmdStatus; }
    std::string GetComments() { return this->response.cmdComment; }
    std::string GetDatasXML() { return this->response.cmdDatasXML; }
    int GetContentId() override { return this->_contentId; }
private:
    int _contentId = -1;
    std::string _contentTitle;
};