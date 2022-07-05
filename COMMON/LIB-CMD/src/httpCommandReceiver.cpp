#include "httpCommandReceiver.h"

std::map<std::string, Session*> HttpCommandHandler::httpSessions;
std::map<std::string, std::shared_ptr<Command>> HttpCommandHandler::commands;
std::map<std::string, std::shared_ptr<CommandResponse>> HttpCommandHandler::responses;
Poco::Mutex HttpCommandHandler::commandsMutex;
Poco::Mutex HttpCommandHandler::responsesMutex;

void HttpCommandHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
{
    // -- get cookies for authentification
    Poco::Net::NameValueCollection cookies;
    req.getCookies(cookies);
    std::string sessionId = "";
    Poco::Net::NameValueCollection::ConstIterator cookieIt = cookies.begin();
    for (; cookieIt != cookies.end(); cookieIt++) {

        if (cookieIt->first == "sessionId") {
            sessionId = cookieIt->second;
        }
    }
    
    if (req.getContentType() == "application/xml") {

        // -- check authentification
        if (req.getURI() == "/login" && req.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
            
            bool authenticated = parseLoginRequest(req.stream());

            if (!authenticated) {
                Poco::Logger::get("HttpCommandHandler").error("Username or password is incorrect !", __FILE__, __LINE__);
                resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNSUPPORTEDMEDIATYPE);
                resp.setReason("Username or password is incorrect !");
                resp.setContentType("application/xml");
                resp.send();
                return;
            }

            else {
                std::map<std::string, Session *>::iterator itSession = HttpCommandHandler::httpSessions.find(sessionId);
                if (itSession != HttpCommandHandler::httpSessions.end()) {
                    itSession->second->reinitTimeout();
                }
                else {
                    Session* session = new Session();
                    HttpCommandHandler::httpSessions.insert_or_assign(session->getId(), session);
                    sessionId = session->getId();
                }

                resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                resp.setContentType("application/xml");
                Poco::Net::HTTPCookie cookie;
                cookie.setHttpOnly(true);
                cookie.setName("sessionId");
                cookie.setValue(sessionId);
                cookie.setPath("/");
                resp.addCookie(cookie);
                resp.send();
                return;
            }
        }

        // -- logout
        if (req.getURI() == "/logout" && req.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
            std::map<std::string, Session *>::iterator itSession;
            itSession = HttpCommandHandler::httpSessions.find(sessionId);
            if (itSession != HttpCommandHandler::httpSessions.end()) {

                delete itSession->second;
                HttpCommandHandler::httpSessions.erase(itSession);
                resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                resp.setReason("Logout !");
                resp.setContentType("application/xml");
                resp.send();
                return;
            }
        }

        // -- all others commands
        else if (req.getURI() == "/" && req.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
            std::map<std::string, Session *>::iterator itSession;
            itSession = HttpCommandHandler::httpSessions.find(sessionId);
            if (itSession != HttpCommandHandler::httpSessions.end()) {

                // -- reset timeout of the session
                itSession->second->reinitTimeout();

                // -- parse the command
                std::shared_ptr<CommandResponse> response = parseCommandRequest(req.stream());
                if (response->getStatus() == CommandResponse::KO) {
                    resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CONFLICT);
                }
                else {
                    resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                }
                resp.setReason(response->getComments());
                // -- send xml datas
                std::ostream& out = resp.send();
                out << response->getDatas();
                out.flush();
                return;
            }
            else {
                Poco::Logger::get("HttpCommandHandler").error("Need authorization !", __FILE__, __LINE__);
                resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                resp.setReason("You need to be authenticated to access this URI !");
                resp.setContentType("application/xml");
                resp.send();
                return;
            }
        }
        else {
            Poco::Logger::get("HttpCommandHandler").error("Nothing to serve at path " + req.getURI() + " !");
            resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
            resp.setReason("You need to be authenticated to access this URI !");
            resp.setContentType("application/xml");
            resp.send();
            return;
        }
    }

    // -- no correct type
    else {
        Poco::Logger::get("HttpCommandHandler").error("Media type " + req.getContentType() + " is not authorized !", __FILE__, __LINE__);
        resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNSUPPORTEDMEDIATYPE);
        resp.setReason("Media type " + req.getContentType() + " is not authorized !");
        resp.setContentType("application/xml");
        resp.send();
        return;
    }
}

bool HttpCommandHandler::parseLoginRequest(std::istream& content) {

    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    Poco::XML::InputSource src(content);
    
    try {
        doc = parser.parse(&src);
    } catch (std::exception &e) {
        Poco::Logger::get("Command").error("Exception when parsing XML file !", __FILE__, __LINE__);
        return false;
    }

    std::string username = "";
    std::string password = "";
    Poco::XML::NodeList* childs = doc->childNodes();
    for (int h = 0; h < childs->length(); h++) {
        if (childs->item(h)->nodeName() == "command") {
            Poco::XML::Node* command = childs->item(h);
            Poco::XML::NodeList* childs2 = command->childNodes();
            for (int i = 0; i < childs2->length(); i++) {

                if (childs2->item(i)->nodeName() == "username") {
                    username = childs2->item(i)->innerText();
                }
                else if (childs2->item(i)->nodeName() == "password") {
                    password = childs2->item(i)->innerText();
                }
            }
            childs2->release();
        }
    }
    childs->release();
    doc->release();
    
    // -- only one password for API
    if (username == "admin" && password == "ice17180") {
        return true;
    }
    else {
        return false;
    }
}

std::shared_ptr<CommandResponse> HttpCommandHandler::parseCommandRequest(std::istream& content)
{
    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command());
    
    if (!cmd->loadFromXmlContent(content)) {
        Poco::Logger::get("HttpCommandHandler").error("Command could not be parsed !", __FILE__, __LINE__);
        std::shared_ptr<CommandResponse> response = std::make_shared<CommandResponse>(CommandResponse(cmd->getUuid(), Command::UNKNOW_COMMAND));
        response->setStatus(CommandResponse::KO);
        response->setComments("Request body parsing error !");
        return response;
    }

    addCommand(cmd);

    // -- try to find response
    int nbRetry = cmd->getNbRetry();
    int waitBetweenRetry = cmd->getWaitBetweenRetry();

    std::shared_ptr<CommandResponse> responseCmd = nullptr;

    bool found = false;
    while (responseCmd == nullptr && nbRetry > 0) {
        nbRetry--;
        Timer::crossUsleep(waitBetweenRetry);
        while (!HttpCommandHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
        std::map<std::string, std::shared_ptr<CommandResponse>>::iterator it = HttpCommandHandler::responses.find(cmd->getUuid());
        if (it != HttpCommandHandler::responses.end()) {
            responseCmd = it->second;
            HttpCommandHandler::responses.erase(it);
            found = true;
        }
        HttpCommandHandler::responsesMutex.unlock();
    }

    // -- no response given
    if (!found) {
        Poco::Logger::get("HttpCommandHandler").error("No response given in time !", __FILE__, __LINE__);
        std::shared_ptr<CommandResponse> response = std::make_shared<CommandResponse>(CommandResponse(cmd->getUuid(), cmd->getType()));
        response->setStatus(CommandResponse::KO);
        response->setComments("No response given in time !");
        return response;
    }

    // -- clean response map
    responseCmd->fillDatas();
    return responseCmd;
}

std::shared_ptr<Command> HttpCommandHandler::getFirstCommand()
{
    std::shared_ptr<Command> cmd = nullptr;
    while (!HttpCommandHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<Command>>::iterator cmdIt = HttpCommandHandler::commands.begin();
    if (cmdIt != HttpCommandHandler::commands.end()) {
        cmd = cmdIt->second;
    }
    HttpCommandHandler::commandsMutex.unlock();
    return cmd;
}
std::shared_ptr<CommandResponse> HttpCommandHandler::getResponse(std::string uuid)
{
    std::shared_ptr<CommandResponse> resp = nullptr;
    while (!HttpCommandHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CommandResponse>>::iterator respIt = HttpCommandHandler::responses.begin();
    if (respIt != HttpCommandHandler::responses.end()) {
        resp = respIt->second;
    }
    HttpCommandHandler::responsesMutex.unlock();
    return resp;
}

void HttpCommandHandler::deleteCommand(std::string uuid)
{
    while (!HttpCommandHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpCommandHandler::commands.erase(uuid);
    HttpCommandHandler::commandsMutex.unlock();
}
void HttpCommandHandler::deleteResponse(std::string uuid)
{
    while (!HttpCommandHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpCommandHandler::responses.erase(uuid);
    HttpCommandHandler::responsesMutex.unlock();
}

void HttpCommandHandler::addResponse(std::shared_ptr<CommandResponse> response)
{
    while (!HttpCommandHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpCommandHandler::responses.insert_or_assign(response->getUuid(), response);
    HttpCommandHandler::responsesMutex.unlock();
}
void HttpCommandHandler::addCommand(std::shared_ptr<Command> command)
{
    while (!HttpCommandHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpCommandHandler::commands.insert_or_assign(command->getUuid(), command);
    HttpCommandHandler::commandsMutex.unlock();
}

void HttpCommandHandler::removeOldResponses() 
{
    while (!HttpCommandHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CommandResponse>>::iterator it;
    for (it =  HttpCommandHandler::responses.begin(); it !=  HttpCommandHandler::responses.end(); it++) {
        // -- no longer available after 25 seconds
        if (Poco::Timestamp().epochMicroseconds() - it->second->getTimestamp() > 3000000) {
            it = HttpCommandHandler::responses.erase(it);
            if (it == HttpCommandHandler::responses.end()) {
                break;
            }
        }
    }
    HttpCommandHandler::responsesMutex.unlock();
}