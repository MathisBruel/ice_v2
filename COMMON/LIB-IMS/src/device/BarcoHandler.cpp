#include "device/BarcoHandler.h"

BarcoHandler::BarcoHandler() : ImsMessageHandler() 
{
    sessionId = "";
    loggedIn = false;
}
BarcoHandler::~BarcoHandler() {}

ImsRequest* BarcoHandler::createLoginRequest(std::string username, std::string password)
{
    std::string params = Poco::format(BarcoHandler::loginParams, username, password);
    std::string function = "login";
    std::string body = Poco::format(BarcoHandler::envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, BarcoHandler::pathAuthenticate, BarcoHandler::contentType);
}
ImsRequest* BarcoHandler::createLogoutRequest()
{
    std::string params = "";
    std::string function = "logout";
    std::string body = Poco::format(BarcoHandler::envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, BarcoHandler::pathAuthenticate, BarcoHandler::contentType);
}

ImsRequest* BarcoHandler::createPlayRequest()
{
    std::string params = "";
    std::string function = "play";
    std::string body = Poco::format(BarcoHandler::envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, BarcoHandler::pathPlayer, BarcoHandler::contentType);
}
ImsRequest* BarcoHandler::createPauseRequest()
{
    std::string params = "";
    std::string function = "pause";
    std::string body = Poco::format(BarcoHandler::envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, BarcoHandler::pathPlayer, BarcoHandler::contentType);
}
ImsRequest* BarcoHandler::createResumeRequest()
{
    std::string params = "";
    std::string function = "resume";
    std::string body = Poco::format(BarcoHandler::envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, BarcoHandler::pathPlayer, BarcoHandler::contentType);
}
ImsRequest* BarcoHandler::createStopRequest()
{
    std::string params = "";
    std::string function = "stop";
    std::string body = Poco::format(BarcoHandler::envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, BarcoHandler::pathPlayer, BarcoHandler::contentType);
}
ImsRequest* BarcoHandler::createJumpTimeCode(std::string timecode) 
{
    if (currentEditRate == -1) {
        Poco::Logger::get("BarcoHandler").error("Edit is not known. We can't jump to timecode that is based on edit rate", __FILE__, __LINE__);
        return nullptr;
    }

    int posEndHour = timecode.find("-", 0);
    if (posEndHour == std::string::npos) {
        Poco::Logger::get("BarcoHandler").error("Time code is not valid", __FILE__, __LINE__);
        return nullptr;
    }
    int posEndMinute = timecode.find("-", posEndHour+1);
    if (posEndMinute == std::string::npos) {
        Poco::Logger::get("BarcoHandler").error("Time code is not valid", __FILE__, __LINE__);
        return nullptr;
    }
    int posEndSecond = timecode.find(".", posEndMinute+1);
    if (posEndSecond == std::string::npos) {
        Poco::Logger::get("BarcoHandler").error("Time code is not valid", __FILE__, __LINE__);
        return nullptr;
    }
    long ms = std::stol(timecode.substr(0, posEndHour))*3600000 + std::stol(timecode.substr(posEndHour+1, posEndMinute))*60000 + 
                std::stol(timecode.substr(posEndMinute+1, posEndSecond))*1000+std::stol(timecode.substr(posEndSecond+1))/currentEditRate;

    // -- time code is formatted as follow : HH-MM-SS.FF
    std::string params = Poco::format(jumpParams, std::to_string(ms));
    std::string function = "changePosition";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}
ImsRequest* BarcoHandler::createGetPlayerStatusRequest()
{
    std::string params = Poco::format(playerStatusParams, sessionId);
    std::string function = "getPlayerStatus";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}

ImsRequest* BarcoHandler::createGetListCplsRequest()
{
    std::string params = "";
    std::string function = "getCplList";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}
ImsRequest* BarcoHandler::createGetCplContentRequest(std::string cplId)
{
    std::string params = Poco::format(cplParams, cplId);
    std::string function = "getCpl";
    std::string body = Poco::format(envelope, function, params);
    return new ImsRequest(Poco::Net::HTTPRequest::HTTP_POST, body, pathPlayer, contentType);
}

void BarcoHandler::handleResponse(std::string jsonString, std::shared_ptr<ImsResponse> response)
{
    //Poco::Logger::get("BarcoHandler").debug("Handle response !", __FILE__, __LINE__);
    //Poco::Logger::get("BarcoHandler").debug(jsonString, __FILE__, __LINE__);

    Poco::Dynamic::Var result;
    try {
        Poco::JSON::Parser parser;
        result = parser.parse(jsonString);
    
    } catch (std::exception &e) {
        Poco::Logger::get("BarcoHandler").error("Exception when parsing json result !", __FILE__, __LINE__);
        Poco::Logger::get("BarcoHandler").error(e.what(), __FILE__, __LINE__);
        return;
    }

    Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
    bool success = object->getValue<bool>("success");
    int errorCode = object->getValue<int>("errorcode");

    if (!success && errorCode == 10508) {
        Poco::Logger::get("BarcoHandler").error("Response not success !", __FILE__, __LINE__);
        loggedIn = false;
        response->setStatus(CommandResponse::KO);
        response->setComments("User is not authenticated !");
        return;
    }

    Poco::JSON::Object::Ptr command = object->getObject("command");
    std::string name = command->getValue<std::string>("name");

    if (name == "login") {
        parseLogin(command->getObject("parameters"), response.get());
    }
    else if (name == "getPlayerStatus") {
        parseShowStatus(command->getObject("parameters")->getObject("playerstatus"), response.get());
    }
    else if (name == "getCplList") {
        parseCplInfos(command->getObject("parameters"), response.get());
    }
    else if (name == "getCpl") {
        parseCplContent(command->getObject("parameters"), response.get());
    }
}

void BarcoHandler::parseLogin(Poco::JSON::Object::Ptr loginParams, ImsResponse* response)
{
    sessionId = loginParams->getValue<std::string>("sessionid");
    response->setStatus(CommandResponse::OK);
    response->setComments("Login success !");
    loggedIn = true;
    //Poco::Logger::get("BarcoHandler").debug("Login success !", __FILE__, __LINE__);
}

void BarcoHandler::parseShowStatus(Poco::JSON::Object::Ptr nodeStatus, ImsResponse* response) 
{
    //Poco::Logger::get("BarcoHandler").debug("Parsing Show status !", __FILE__, __LINE__);
    PlayerStatus status;
    std::shared_ptr<PlayerStatus> playerStatus = std::make_shared<PlayerStatus>(status);
    // -- we set the timestamp to know if coherent times
    playerStatus->setTimestamp(Poco::Timestamp().epochMicroseconds());

    playerStatus->setCplId(nodeStatus->getValue<std::string>("selectionId"));
    playerStatus->setCplTitle(nodeStatus->getValue<std::string>("title"));

    std::string editRate = nodeStatus->getValue<std::string>("currentClipEditRate");
    int posSpace = editRate.find(" ", 0);
    int num = std::stoi(editRate.substr(0, posSpace));
    int den = std::stoi(editRate.substr(posSpace+1));
    playerStatus->setspeedRate(num, den);

    playerStatus->setEditPosition(nodeStatus->getValue<int>("currentClipFrameIndex"));
    int stateInt = nodeStatus->getValue<int>("state");
    PlayerStatus::State state = PlayerStatus::UnknownState;
    if (stateInt == 4) {
        state = PlayerStatus::Stop;
    }
    else if (stateInt == 5) {
        state = PlayerStatus::Play;
    }
    else if (stateInt == 6) {
        state = PlayerStatus::Pause;
    }
    playerStatus->setState(state);

    response->setStatus(CommandResponse::OK);
    response->setComments("Get player Status achived !");
    response->setStatusPlayer(playerStatus);
}

void BarcoHandler::parseCplInfos(Poco::JSON::Object::Ptr nodeCplInfoResponse, ImsResponse* response)
{
    Poco::Logger::get("BarcoHandler").debug("Parsing Cpl infos !", __FILE__, __LINE__);
    Poco::JSON::Array::Ptr CPLS = nodeCplInfoResponse->getObject("cplinformation")->getArray("cpllist");
    for (int i = 0; i < CPLS->size(); i++) {
        Poco::Dynamic::Var cplVar = CPLS->get(i);
        Poco::JSON::Object::Ptr cpl = cplVar.extract<Poco::JSON::Object::Ptr>();

        std::shared_ptr<CplInfos> info = std::make_shared<CplInfos>(CplInfos());
        info->setCplId(cpl->getValue<std::string>("id"));
        info->setCplTitle(cpl->getValue<std::string>("title"));
        info->setCplType(CplFile::parseCplTypeFromString(cpl->getValue<std::string>("contentKind")));
        info->setCplDuration(cpl->getValue<int>("durationInFrames"));
        info->setPictureWidth(cpl->getValue<int>("mainPictureActiveWidth"));

        bool is3D =  cpl->getValue<std::string>("mainPictureActiveWidth") != "2D";
        info->set3D(is3D);
        info->setPlayable(true);
        info->setcontentVersionId(cpl->getValue<std::string>("contentVersionId"));

        int num = cpl->getValue<int>("mainPictureEditRateNumerator");
        int den = cpl->getValue<int>("mainPictureEditRateDenominator");
        info->setSpeedRate((double)num/(double)den);

        response->addNewCpl(info->getCplId(), info);
    }

    response->setStatus(CommandResponse::OK);
    response->setComments("Get cpl Info achived !");
}

void BarcoHandler::parseCplContent(Poco::JSON::Object::Ptr nodeCplContentResponse, ImsResponse* response)
{
    //Poco::Logger::get("BarcoHandler").debug("Parsing Cpl content !", __FILE__, __LINE__);
    response->setStatus(CommandResponse::OK);
    response->setComments("Get cpl content achived !");
    response->setContentFile(nodeCplContentResponse->getValue<std::string>("cpl"));
}