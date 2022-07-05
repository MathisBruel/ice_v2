#include "device/DoremiHandler.h"

DoremiHandler::DoremiHandler() : ImsMessageHandler() 
{
    sessionId = "";
}
DoremiHandler::~DoremiHandler() {}

ImsRequest* DoremiHandler::createLoginRequest(std::string username, std::string password)
{
    std::string params = Poco::format(loginParams, username, password);
    std::string function = "Login";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathAuthenticate, contentType);
}
ImsRequest* DoremiHandler::createLogoutRequest()
{
    if (sessionId == "") {
        return nullptr;
    }

    std::string params = Poco::format(sessionParams, sessionId);
    std::string function = "Logout";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathAuthenticate, contentType);
}

ImsRequest* DoremiHandler::createPlayRequest()
{
    if (sessionId == "") {
        return nullptr;
    }

    std::string params = Poco::format(sessionParams, sessionId);
    std::string function = "Play";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}
ImsRequest* DoremiHandler::createPauseRequest()
{
    if (sessionId == "") {
        return nullptr;
    }

    std::string params = Poco::format(sessionParams, sessionId);
    std::string function = "Pause";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}
ImsRequest* DoremiHandler::createResumeRequest()
{
    return createPlayRequest();
}
ImsRequest* DoremiHandler::createStopRequest()
{
    std::string params = Poco::format(sessionParams, sessionId);
    std::string function = "Eject";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}
ImsRequest* DoremiHandler::createJumpTimeCode(std::string timecode) 
{
    // -- time code is formatted as follow : HH-MM-SS.FF
    std::string params = Poco::format(sessionParams, sessionId) + "<timecode>" + timecode + "</timecode>";
    std::string function = "JumpTimecode";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}
ImsRequest* DoremiHandler::createGetPlayerStatusRequest()
{
    if (sessionId == "") {
        return nullptr;
    }

    std::string params = Poco::format(sessionParams, sessionId);
    std::string function = "GetShowStatus";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}

ImsRequest* DoremiHandler::createGetListCplsRequest()
{
    if (sessionId == "") {
        return nullptr;
    }

    std::string params = Poco::format(sessionParams, sessionId);
    std::string function = "GetCPLList";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathCpl, contentType);
}
ImsRequest* DoremiHandler::createGetCplInfoRequest(std::string cplId)
{
    if (sessionId == "") {
        return nullptr;
    }

    std::string params = Poco::format(cplParams, sessionId, cplId);
    std::string function = "GetCPLInfo";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathCpl, contentType);
}
ImsRequest* DoremiHandler::createGetCplContentRequest(std::string cplId)
{
    if (sessionId == "") {
        return nullptr;
    }

    std::string params = Poco::format(cplParams, sessionId, "urn:uuid:" + cplId);
    std::string function = "GetCPLContent";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathCpl, contentType);
}

bool invalidChar (char c) 
{  
    return c < 9;   
} 

void DoremiHandler::handleResponse(std::string xmlString, std::shared_ptr<ImsResponse> response)
{
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;

    xmlString.erase(std::remove_if(xmlString.begin(),xmlString.end(), invalidChar), xmlString.end());
    
    try {
        doc = parser.parseString(xmlString);
    } 
    catch (std::exception &e) {
        Poco::Logger::get("DoremiHandler").error("Error while parsing response", __FILE__, __LINE__);
        Poco::Logger::get("DoremiHandler").error(e.what(), __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Error while parsing response");
        return;
    }

    Poco::XML::NodeList* bodyList = doc->getElementsByTagName("SOAP-ENV:Body");

    // -- we check format of body : only 1 body
    if (bodyList == nullptr) {
        Poco::Logger::get("DoremiHandler").error("Incorrect HTTP response : no body", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Incorrect HTTP response : no body");
        doc->release();
        return;
    }
    if (bodyList->length() != 1) {
        Poco::Logger::get("DoremiHandler").error("Incorrect HTTP response : more than 1 body !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Incorrect HTTP response : more than 1 body !");
        bodyList->release();
        doc->release();
        return;
    }

    // -- we check elements of the body : only 1 element
    Poco::XML::NodeList* childs = bodyList->item(0)->childNodes();
    for (int i = 0; i < childs->length(); i++) {

        // -- we get element that contains type of the response
        Poco::XML::Node* nodeType = childs->item(i);

        // -- LOGIN
        if (nodeType->nodeName().find("LoginResponse", 0) != std::string::npos) {
            parseLogin(nodeType, response.get());
        }

        else if (nodeType->nodeName().find("LogoutResponse", 0) != std::string::npos) {
            loggedIn = false;
            sessionId = "";
            response->setStatus(CommandResponse::OK);
            response->setComments("Logout achieved !");
        }

        else if (nodeType->nodeName().find("GetShowStatusResponse", 0) != std::string::npos) {
            parseShowStatus(nodeType, response.get());
        }

        else if (nodeType->nodeName().find("GetCPLListResponse", 0) != std::string::npos) {
            parseListCpl(nodeType, response.get());
        }
        else if (nodeType->nodeName().find("GetCPLInfoResponse", 0) != std::string::npos) {
            parseCplInfos(nodeType, response.get());
        }
        else if (nodeType->nodeName().find("GetCPLContentResponse", 0) != std::string::npos) {
            parseCplContent(nodeType, response.get());
        }
        else if (nodeType->nodeName().find("Fault", 0) != std::string::npos) {
            parseFault(nodeType, response.get());
        }
    
    }
    childs->release();
    bodyList->release();
    doc->release();
}

void DoremiHandler::parseFault(Poco::XML::Node* nodeFault, ImsResponse* response)
{
    response->setStatus(CommandResponse::KO);

    // -- must have a sessionId node
    if (!nodeFault->hasChildNodes()) {
        Poco::Logger::get("DoremiHandler").error("HTTP response to fault does not contain explanation !", __FILE__, __LINE__);
        response->setComments("HTTP response to fault does not contain explanation !");
        return;
    }

    // -- parse node in LoginResponse
    bool found = false;
    Poco::XML::NodeList* childs = nodeFault->childNodes();
    for (int j = 0; j < childs->length(); j++) {
        Poco::XML::Node* nodeSession = childs->item(j);

        // -- find sessionId
        if (nodeSession->nodeName() == "faultstring") {
            if (nodeSession->innerText() == "not authenticated") {
                response->setComments("Not authenticated : reconnect");
                sessionId = "";
                loggedIn = false;
            }
            break;
        }
    }
    childs->release();
}

void DoremiHandler::parseLogin(Poco::XML::Node* nodeLogin, ImsResponse* response)
{
    // -- must have a sessionId node
    if (!nodeLogin->hasChildNodes()) {
        Poco::Logger::get("DoremiHandler").error("HTTP response to login does not contain sessionId !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("HTTP response to login does not contain sessionId !");
        return;
    }

    // -- parse node in LoginResponse
    bool found = false;
    Poco::XML::NodeList* childs = nodeLogin->childNodes();
    for (int j = 0; j < childs->length(); j++) {
        Poco::XML::Node* nodeSession = childs->item(j);

        // -- find sessionId
        if (nodeSession->nodeName() == "sessionId") {
            sessionId = nodeSession->innerText();
            found = true;
            loggedIn = true;
            break;
        }
    }
    childs->release();

    if (!found) {
        Poco::Logger::get("DoremiHandler").error("HTTP response to login does not contain sessionId !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("HTTP response to login does not contain sessionId !");
        return;
    }

    response->setStatus(CommandResponse::OK);
    response->setComments("Login achived !");
}

void DoremiHandler::parseShowStatus(Poco::XML::Node* nodeStatus, ImsResponse* response) {

    // -- must have a sessionId node
    if (!nodeStatus->hasChildNodes()) {
        Poco::Logger::get("DoremiHandler").error("HTTP response to show status does not contain informations !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("HTTP response to show status does not contain informations !");
        return;
    }

    PlayerStatus status;
    std::shared_ptr<PlayerStatus> playerStatus = std::make_shared<PlayerStatus>(status);

    // -- we set the timestamp to know if coherent times
    playerStatus->setTimestamp(Poco::Timestamp().epochMicroseconds());

    bool found = false;
    Poco::XML::NodeList* childs = nodeStatus->childNodes();
    for (int j = 0; j < childs->length(); j++) {
        Poco::XML::Node* nodeShowStatus = childs->item(j);

        // -- find showStatus tag
        if (nodeShowStatus->nodeName() == "showStatus") {

            playerStatus->setEditPosition(0);
            playerStatus->setCplTitle("");
            
            // -- parse and fill all usefull fields
            Poco::XML::NodeList* childs2 = nodeShowStatus->childNodes();
            for (int k = 0; k < childs2->length(); k++) {
                Poco::XML::Node* item = childs2->item(k);
                
                // -- list of possible fields
                if (item->nodeName().find("stateInfo", 0) != std::string::npos) 
                {
                    playerStatus->setState(PlayerStatus::parseStateFromString(item->innerText()));
                }
                else if (item->nodeName().find("cplId", 0) != std::string::npos) 
                {
                    std::string cplIdRaw = item->innerText();
                    int pos = cplIdRaw.find_last_of(":");
                    playerStatus->setCplId(cplIdRaw.substr(pos+1));
                }
                else if (item->nodeName().find("cplTitle", 0) != std::string::npos) 
                {
                    playerStatus->setCplTitle(item->innerText());
                }
                else if (item->nodeName().find("editPosition", 0) != std::string::npos) 
                {
                    playerStatus->setEditPosition(std::stoi(item->innerText()));
                }
                else if (item->nodeName().find("editRate", 0) != std::string::npos) 
                {
                    int num = 24;
                    int den = 1;

                    Poco::XML::NodeList* childs3 = item->childNodes();
                    try {
                        for (int l = 0; l < childs3->length(); l++) {
                            Poco::XML::Node* rateItem = childs3->item(l);
                            if (rateItem->nodeName().find("numerator", 0) != std::string::npos) {
                                num = std::stoi(rateItem->innerText());
                            }
                            else if (rateItem->nodeName().find("denominator", 0) != std::string::npos) {
                                den = std::stoi(rateItem->innerText());
                            }
                        }
                        playerStatus->setspeedRate(num, den);
                    } catch (std::exception &e) {
                        Poco::Logger::get("DoremiHandler").error("Exception when parsing editRate : values are not integers !", __FILE__, __LINE__);
                        response->setStatus(CommandResponse::KO);
                        response->setComments("Exception when parsing editRate : values are not integers !");
                    }
                    childs3->release();
                }
            }
            childs2->release();
            found = true;
            break;
        }
    }
    childs->release();

    if (!found) {
        Poco::Logger::get("DoremiHandler").error("HTTP response to get show status does not contain showStatus !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("HTTP response to get show status does not contain showStatus !");
    }

    response->setStatus(CommandResponse::OK);
    response->setComments("Get player Status achived !");
    response->setStatusPlayer(playerStatus);
}

void DoremiHandler::parseListCpl(Poco::XML::Node* nodeCplListResponse, ImsResponse* response)
{
    bool foundNode = false;
    Poco::XML::NodeList* childs = nodeCplListResponse->childNodes();
    for (int j = 0; j < childs->length(); j++) {

        Poco::XML::Node* nodeCplListId = childs->item(j);
        if (nodeCplListId->nodeName() == "cplList") {

            // -- now we get a real list of string cplId
            Poco::XML::NodeList* childs2 = nodeCplListId->childNodes();
            for (int k = 0; k < childs2->length(); k++) {

                Poco::XML::Node* nodeCplId = childs2->item(k);
                if (nodeCplId->nodeName().find("cplId", 0) != std::string::npos) {
                    std::string cplId = nodeCplId->innerText();
                    response->addNewCpl(cplId, nullptr);
                }
            }
            childs2->release();
            foundNode = true;
            break;
        }
    }
    childs->release();

    if (!foundNode) {
        Poco::Logger::get("DoremiHandler").error("No node cplList in response !");
        response->setStatus(CommandResponse::KO);
        response->setComments("No node CplId in response !");
    }
    else {
        response->setStatus(CommandResponse::OK);
        response->setComments("Get list cplId achived !");
    }
}

void DoremiHandler::parseCplInfos(Poco::XML::Node* nodeCplInfoResponse, ImsResponse* response)
{
    bool found = false;

    Poco::XML::NodeList* childs = nodeCplInfoResponse->childNodes();
    for (int j = 0; j < childs->length(); j++) {

        // -- try to fin node cplInfo
        Poco::XML::Node* nodeCplInfo = childs->item(j);
        if (nodeCplInfo->nodeName() == "cplInfo") {

            std::shared_ptr<CplInfos> info = std::make_shared<CplInfos>(CplInfos());
            std::string uuid;
            found = true;

            Poco::XML::NodeList* childs2 = nodeCplInfo->childNodes();
            for (int k = 0; k < childs2->length(); k++) {

                try {
                    // -- parse all usefull fields
                    Poco::XML::Node* nodeCplInfoItem = childs2->item(k);
                    if (nodeCplInfoItem->nodeName().find("uuid", 0) != std::string::npos) {
                        int pos = nodeCplInfoItem->innerText().find_last_of(":");
                        info->setCplId(nodeCplInfoItem->innerText().substr(pos+1));
                        uuid = nodeCplInfoItem->innerText();
                    }
                    else if (nodeCplInfoItem->nodeName().find("durationEdits", 0) != std::string::npos) {
                        info->setCplDuration(std::stoi(nodeCplInfoItem->innerText()));
                    }
                    else if (nodeCplInfoItem->nodeName().find("contentTitleText", 0) != std::string::npos) {
                        info->setCplTitle(nodeCplInfoItem->innerText());
                    }
                    else if (nodeCplInfoItem->nodeName().find("contentKind", 0) != std::string::npos) {
                        info->setCplType(CplFile::parseCplTypeFromString(nodeCplInfoItem->innerText()));
                    }
                    else if (nodeCplInfoItem->nodeName().find("pictureWidth", 0) != std::string::npos) {
                        info->setPictureWidth(std::stoi(nodeCplInfoItem->innerText()));
                    }
                    else if (nodeCplInfoItem->nodeName().find("editRate", 0) != std::string::npos) {
                        int num = 24;
                        int den = 1;

                        Poco::XML::NodeList* childs3 = nodeCplInfoItem->childNodes();
                        for (int l = 0; l < childs3->length(); l++) {
                            Poco::XML::Node* rateItem = childs3->item(l);
                            if (rateItem->nodeName().find("numerator", 0) != std::string::npos) {
                                num = std::stoi(rateItem->innerText());
                            }
                            else if (rateItem->nodeName().find("denominator", 0) != std::string::npos) {
                                num = std::stoi(rateItem->innerText());
                            }
                        }
                        childs3->release();
                        info->setSpeedRate((double)num/(double)den);
                    }
                    else if (nodeCplInfoItem->nodeName().find("is3D", 0) != std::string::npos) {

                        if (nodeCplInfoItem->innerText() == "true") {
                            info->set3D(true);
                        } 
                        else {
                            info->set3D(false);
                        }
                    }
                    else if (nodeCplInfoItem->nodeName().find("contentVersionId", 0) != std::string::npos) {
                        info->setcontentVersionId(nodeCplInfoItem->innerText());
                    }
                    else if (nodeCplInfoItem->nodeName().find("playable", 0) != std::string::npos) {
                        if (nodeCplInfoItem->innerText() == "true") {
                            info->setPlayable(true);
                        } 
                        else {
                            info->setPlayable(false);
                        }
                    }
                    else if (nodeCplInfoItem->nodeName().find("propertyList", 0) != std::string::npos) {

                        Poco::XML::NodeList* childs3 = nodeCplInfoItem->childNodes();
                        for (int l = 0; l < childs3->length(); l++) {
                            Poco::XML::Node* propertyList = childs3->item(l);
                            if (propertyList->nodeName().find("property", 0) != std::string::npos) {
                                info->addProperties(CplFile::parseCplPropertyFromString(propertyList->innerText()));
                            }
                        }
                        childs3->release();
                    }

                } catch (std::exception &e) {
                    Poco::Logger::get("DoremiHandler").error("Exception when parsing cplInfo : values are not integers !", __FILE__, __LINE__);
                    response->setStatus(CommandResponse::KO);
                    response->setComments("Exception when parsing cplInfo : values are not integers !");
                }
            }
            childs2->release();

            // -- fill in map
            response->addNewCpl(uuid, info);
            info.reset();
            response->setStatus(CommandResponse::OK);
            response->setComments("Get cpl Info achived !");
        }
    }
    childs->release();

    if (!found) {
        Poco::Logger::get("DoremiHandler").error("No node cplInfo in response !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("No node cplInfo in response !");
    }
}

void DoremiHandler::parseCplContent(Poco::XML::Node* nodeCplContentResponse, ImsResponse* response)
{
    bool found = false;
    Poco::XML::NodeList* childs = nodeCplContentResponse->childNodes();
    for (int j = 0; j < childs->length(); j++) {

        // -- try to fin node cplInfo
        Poco::XML::Node* nodeCplContent = childs->item(j);
        if (nodeCplContent->nodeName() == "content") {
        
            found = true;

            // -- decode content
            std::string decodedXmlString = Converter::decodeBase64(nodeCplContent->innerText());
    
            response->setStatus(CommandResponse::OK);
            response->setComments("Get cpl content achived !");
            response->setContentFile(decodedXmlString);
        }
    }

    childs->release();

    if (!found) {
        Poco::Logger::get("DoremiHandler").error("No node content in response !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("No node content in response !");
    }
}