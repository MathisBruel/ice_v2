#include <cstdlib>
#include <iostream>

#pragma once
#include "Converter.h"
#include "ImsRequest.h"
#include "commandResponse.h"
#include "xml/PlayerStatus.h"
#include "xml/CplInfos.h"

class ImsResponse : public CommandResponse
{

public:

    ImsResponse(std::string uuidFromCommand, Command::CommandType type);
    void setStatusPlayer(std::shared_ptr<PlayerStatus> statusPlayer) {this->statusPlayer = statusPlayer;}
    void setContentFile(std::string contentFile) {this->contentFile = contentFile;}
    void addNewCpl(std::string cplId, std::shared_ptr<CplInfos> info) {listOfCpls.insert_or_assign(cplId, info);}

    ~ImsResponse();

    std::shared_ptr<PlayerStatus> getPlayerStatus() {return statusPlayer;}
    std::map<std::string, std::shared_ptr<CplInfos>> getListOfCpls() {return listOfCpls;}
    std::string getContentFile() {return contentFile;}
    void clearMap() {listOfCpls.clear();}

    void fillDatas();

private:

    void createCplContentXml(Poco::XML::Document* doc);
    void createCplContentNameXml(Poco::XML::Document* doc);

    std::shared_ptr<PlayerStatus> statusPlayer;
    std::map<std::string, std::shared_ptr<CplInfos>> listOfCpls;
    std::string contentFile;
};