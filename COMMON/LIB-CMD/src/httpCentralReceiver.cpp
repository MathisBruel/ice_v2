#include "httpCentralReceiver.h"

std::map<std::string, Session*> HttpCentralHandler::httpSessions;
std::map<std::string, std::shared_ptr<CommandCentral>> HttpCentralHandler::commands;
std::map<std::string, std::shared_ptr<CommandCentralResponse>> HttpCentralHandler::responses;
Poco::Mutex HttpCentralHandler::commandsMutex;
Poco::Mutex HttpCentralHandler::responsesMutex;

void HttpCentralHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
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
                Poco::Logger::get("HttpCentralHandler").error("Username or password is incorrect !", __FILE__, __LINE__);
                resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNSUPPORTEDMEDIATYPE);
                resp.setReason("Username or password is incorrect !");
                resp.setContentType("application/xml");
                resp.send();
                return;
            }

            else {
                std::map<std::string, Session *>::iterator itSession = HttpCentralHandler::httpSessions.find(sessionId);
                if (itSession != HttpCentralHandler::httpSessions.end()) {
                    itSession->second->reinitTimeout();
                }
                else {
                    Session* session = new Session();
                    HttpCentralHandler::httpSessions.insert_or_assign(session->getId(), session);
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
            itSession = HttpCentralHandler::httpSessions.find(sessionId);
            if (itSession != HttpCentralHandler::httpSessions.end()) {

                delete itSession->second;
                HttpCentralHandler::httpSessions.erase(itSession);
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
            itSession = HttpCentralHandler::httpSessions.find(sessionId);
            if (itSession != HttpCentralHandler::httpSessions.end()) {

                // -- reset timeout of the session
                itSession->second->reinitTimeout();

                // -- parse the command
                std::shared_ptr<CommandCentralResponse> response = parseCommandRequest(req.stream());
                if (response->getStatus() == CommandCentralResponse::KO) {
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
                Poco::Logger::get("HttpCentralHandler").error("Need authorization !", __FILE__, __LINE__);
                resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                resp.setReason("You need to be authenticated to access this URI !");
                resp.setContentType("application/xml");
                resp.send();
                return;
            }
        }
        else {
            Poco::Logger::get("HttpCentralHandler").error("Nothing to serve at path " + req.getURI() + " !");
            resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
            resp.setReason("You need to be authenticated to access this URI !");
            resp.setContentType("application/xml");
            resp.send();
            return;
        }
    }

    // -- no correct type
    else {
        Poco::Logger::get("HttpCentralHandler").error("Media type " + req.getContentType() + " is not authorized !", __FILE__, __LINE__);
        resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNSUPPORTEDMEDIATYPE);
        resp.setReason("Media type " + req.getContentType() + " is not authorized !");
        resp.setContentType("application/xml");
        resp.send();
        return;
    }
}

bool HttpCentralHandler::parseLoginRequest(std::istream& content) {

    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    Poco::XML::InputSource src(content);

    try {
        doc = parser.parse(&src);
    } catch (std::exception &e) {
        Poco::Logger::get("HttpCentralHandler").error("Exception when parsing XML file !", __FILE__, __LINE__);
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

std::shared_ptr<CommandCentralResponse> HttpCentralHandler::parseCommandRequest(std::istream& content)
{
    std::shared_ptr<CommandCentral> cmd = std::make_shared<CommandCentral>(CommandCentral());
    
    if (!cmd->loadFromXmlContent(content)) {
        Poco::Logger::get("HttpCentralHandler").error("Command could not be parsed !", __FILE__, __LINE__);
        std::shared_ptr<CommandCentralResponse> response = std::make_shared<CommandCentralResponse>(CommandCentralResponse(cmd->getUuid(), CommandCentral::UNKNOW_COMMAND));
        response->setStatus(CommandCentralResponse::KO);
        response->setComments("Request body parsing error !");
        return response;
    }

    addCommand(cmd);

    // -- try to find response
    int nbRetry = cmd->getNbRetry();
    int waitBetweenRetry = cmd->getWaitBetweenRetry();

    std::shared_ptr<CommandCentralResponse> responseCmd = nullptr;

    bool found = false;
    while (responseCmd == nullptr && nbRetry > 0) {
        nbRetry--;
        Timer::crossUsleep(waitBetweenRetry);
        while (!HttpCentralHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
        std::map<std::string, std::shared_ptr<CommandCentralResponse>>::iterator it = HttpCentralHandler::responses.find(cmd->getUuid());
        if (it != HttpCentralHandler::responses.end()) {
            responseCmd = it->second;
            HttpCentralHandler::responses.erase(it);
            found = true;
        }
        HttpCentralHandler::responsesMutex.unlock();
    }

    // -- no response given
    if (!found) {
        Poco::Logger::get("HttpCentralHandler").error("No response given in time !", __FILE__, __LINE__);
        std::shared_ptr<CommandCentralResponse> response = std::make_shared<CommandCentralResponse>(CommandCentralResponse(cmd->getUuid(), cmd->getType()));
        response->setStatus(CommandCentralResponse::KO);
        response->setComments("No response given in time !");
        return response;
    }

    return responseCmd;
}

std::shared_ptr<CommandCentral> HttpCentralHandler::getFirstCommand()
{
    std::shared_ptr<CommandCentral> cmd = nullptr;
    while (!HttpCentralHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CommandCentral>>::iterator cmdIt = HttpCentralHandler::commands.begin();
    if (cmdIt != HttpCentralHandler::commands.end()) {
        cmd = cmdIt->second;
    }
    HttpCentralHandler::commandsMutex.unlock();
    return cmd;
}
std::shared_ptr<CommandCentralResponse> HttpCentralHandler::getResponse(std::string uuid)
{
    std::shared_ptr<CommandCentralResponse> resp = nullptr;
    while (!HttpCentralHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CommandCentralResponse>>::iterator respIt = HttpCentralHandler::responses.begin();
    if (respIt != HttpCentralHandler::responses.end()) {
        resp = respIt->second;
    }
    HttpCentralHandler::responsesMutex.unlock();
    return resp;
}

void HttpCentralHandler::deleteCommand(std::string uuid)
{
    while (!HttpCentralHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpCentralHandler::commands.erase(uuid);
    HttpCentralHandler::commandsMutex.unlock();
}
void HttpCentralHandler::deleteResponse(std::string uuid)
{
    while (!HttpCentralHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpCentralHandler::responses.erase(uuid);
    HttpCentralHandler::responsesMutex.unlock();
}

void HttpCentralHandler::addResponse(std::shared_ptr<CommandCentralResponse> response)
{
    while (!HttpCentralHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpCentralHandler::responses.insert_or_assign(response->getUuid(), response);
    HttpCentralHandler::responsesMutex.unlock();
}
void HttpCentralHandler::addCommand(std::shared_ptr<CommandCentral> command)
{
    while (!HttpCentralHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpCentralHandler::commands.insert_or_assign(command->getUuid(), command);
    HttpCentralHandler::commandsMutex.unlock();
}

void HttpCentralHandler::removeOldResponses() 
{
    while (!HttpCentralHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CommandCentralResponse>>::iterator it;
    for (it =  HttpCentralHandler::responses.begin(); it !=  HttpCentralHandler::responses.end(); it++) {
        // -- no longer available after 25 seconds
        if (Poco::Timestamp().epochMicroseconds() - it->second->getTimestamp() > 3000000) {
            it = HttpCentralHandler::responses.erase(it);
            if (it == HttpCentralHandler::responses.end()) {
                break;
            }
        }
    }
    HttpCentralHandler::responsesMutex.unlock();
}