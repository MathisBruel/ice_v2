#include "httpRepoReceiver.h"

std::map<std::string, Session*> HttpRepoHandler::httpSessions;
std::map<std::string, std::shared_ptr<CommandCentral>> HttpRepoHandler::commands;
std::map<std::string, std::shared_ptr<CommandCentralResponse>> HttpRepoHandler::responses;
Poco::Mutex HttpRepoHandler::commandsMutex;
Poco::Mutex HttpRepoHandler::responsesMutex;

void HttpRepoHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
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
                Poco::Logger::get("HttpRepoHandler").error("Username or password is incorrect !", __FILE__, __LINE__);
                resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNSUPPORTEDMEDIATYPE);
                resp.setReason("Username or password is incorrect !");
                resp.setContentType("application/xml");
                resp.send();
                return;
            }

            else {
                std::map<std::string, Session *>::iterator itSession = HttpRepoHandler::httpSessions.find(sessionId);
                if (itSession != HttpRepoHandler::httpSessions.end()) {
                    itSession->second->reinitTimeout();
                }
                else {
                    Session* session = new Session();
                    HttpRepoHandler::httpSessions.insert_or_assign(session->getId(), session);
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
            itSession = HttpRepoHandler::httpSessions.find(sessionId);
            if (itSession != HttpRepoHandler::httpSessions.end()) {

                delete itSession->second;
                HttpRepoHandler::httpSessions.erase(itSession);
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
            itSession = HttpRepoHandler::httpSessions.find(sessionId);
            if (itSession != HttpRepoHandler::httpSessions.end()) {

                // -- reset timeout of the session
                itSession->second->reinitTimeout();

                // -- parse the command
                std::shared_ptr<CommandCentral> cmd = std::make_shared<CommandCentral>(CommandCentral());
                if (!cmd->loadFromXmlContent(req.stream())) {
                    Poco::Logger::get("HttpRepoHandler").error("Command could not be parsed !", __FILE__, __LINE__);
                    resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CONFLICT);
                    resp.setReason("Command could not be parsed !");
                    // -- send xml datas
                    std::ostream& out = resp.send();
                    out.flush();
                }
                else if (cmd->getType() != CommandCentral::GET_FILE) {
                    resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CONFLICT);
                    resp.setReason("Command is not GET_FILE type !");
                    // -- send xml datas
                    std::ostream& out = resp.send();
                    out.flush();
                }
                else {

                    std::string filename = cmd->getStringParameter("filePath");
                    int offset = cmd->getIntParameter("offset", 0);

                    std::ostream& out = resp.send();
                    std::ifstream is(filename, std::ios_base::in | std::ios_base::binary);
                    if (is) {

                        resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                        resp.setReason("Downloading file");

                        is.seekg (0, is.end);
                        long size = is.tellg();
                        is.seekg(offset, is.beg);

                        // -- copy 1024 bytes at the time
                        char* buffer = new char[1024];
                        int nbBytesToCopy = size - offset;

                        int copyIdx = 0;
                        while (copyIdx < nbBytesToCopy) {

                            if (nbBytesToCopy - copyIdx >= 1024) {
                                is.read(buffer, 1024);
                                out.write(buffer, 1024);
                                copyIdx += 1024;
                            }
                            else {
                                is.read(buffer, nbBytesToCopy - copyIdx);
                                out.write(buffer, nbBytesToCopy - copyIdx);
                                copyIdx = nbBytesToCopy;
                            }
                        }

                        // release dynamically-allocated memory
                        delete[] buffer;
                    }
                    else {
                        resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CONFLICT);
                        resp.setReason("Could not read file !");
                    }

                    out.flush();
                    is.close();
                }
                return;
            }
            else {
                Poco::Logger::get("HttpRepoHandler").error("Need authorization !", __FILE__, __LINE__);
                resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                resp.setReason("You need to be authenticated to access this URI !");
                resp.setContentType("application/xml");
                resp.send();
                return;
            }
        }
        else {
            Poco::Logger::get("HttpRepoHandler").error("Nothing to serve at path " + req.getURI() + " !");
            resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
            resp.setReason("You need to be authenticated to access this URI !");
            resp.setContentType("application/xml");
            resp.send();
            return;
        }
    }

    // -- no correct type
    else {
        Poco::Logger::get("HttpRepoHandler").error("Media type " + req.getContentType() + " is not authorized !", __FILE__, __LINE__);
        resp.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNSUPPORTEDMEDIATYPE);
        resp.setReason("Media type " + req.getContentType() + " is not authorized !");
        resp.setContentType("application/xml");
        resp.send();
        return;
    }
}

bool HttpRepoHandler::parseLoginRequest(std::istream& content) {

    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    Poco::XML::InputSource src(content);
    
    try {
        doc = parser.parse(&src);
    } catch (std::exception &e) {
        Poco::Logger::get("HttpRepoHandler").error("Exception when parsing XML file !", __FILE__, __LINE__);
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

std::shared_ptr<CommandCentral> HttpRepoHandler::getFirstCommand()
{
    std::shared_ptr<CommandCentral> cmd = nullptr;
    while (!HttpRepoHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CommandCentral>>::iterator cmdIt = HttpRepoHandler::commands.begin();
    if (cmdIt != HttpRepoHandler::commands.end()) {
        cmd = cmdIt->second;
    }
    HttpRepoHandler::commandsMutex.unlock();
    return cmd;
}
std::shared_ptr<CommandCentralResponse> HttpRepoHandler::getResponse(std::string uuid)
{
    std::shared_ptr<CommandCentralResponse> resp = nullptr;
    while (!HttpRepoHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CommandCentralResponse>>::iterator respIt = HttpRepoHandler::responses.begin();
    if (respIt != HttpRepoHandler::responses.end()) {
        resp = respIt->second;
    }
    HttpRepoHandler::responsesMutex.unlock();
    return resp;
}

void HttpRepoHandler::deleteCommand(std::string uuid)
{
    while (!HttpRepoHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpRepoHandler::commands.erase(uuid);
    HttpRepoHandler::commandsMutex.unlock();
}
void HttpRepoHandler::deleteResponse(std::string uuid)
{
    while (!HttpRepoHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpRepoHandler::responses.erase(uuid);
    HttpRepoHandler::responsesMutex.unlock();
}

void HttpRepoHandler::addResponse(std::shared_ptr<CommandCentralResponse> response)
{
    while (!HttpRepoHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpRepoHandler::responses.insert_or_assign(response->getUuid(), response);
    HttpRepoHandler::responsesMutex.unlock();
}
void HttpRepoHandler::addCommand(std::shared_ptr<CommandCentral> command)
{
    while (!HttpRepoHandler::commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    HttpRepoHandler::commands.insert_or_assign(command->getUuid(), command);
    HttpRepoHandler::commandsMutex.unlock();
}

void HttpRepoHandler::removeOldResponses() 
{
    while (!HttpRepoHandler::responsesMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<CommandCentralResponse>>::iterator it;
    for (it =  HttpRepoHandler::responses.begin(); it !=  HttpRepoHandler::responses.end(); it++) {
        // -- no longer available after 25 seconds
        if (Poco::Timestamp().epochMicroseconds() - it->second->getTimestamp() > 3000000) {
            it = HttpRepoHandler::responses.erase(it);
            if (it == HttpRepoHandler::responses.end()) {
                break;
            }
        }
    }
    HttpRepoHandler::responsesMutex.unlock();
}