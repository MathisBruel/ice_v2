#include "ImsCommunicator.h"

ImsCommunicator::ImsCommunicator(std::string host, uint16_t port, ImsRequest::Protocol protocol)
{
    this->host = host;
    this->port = port;
    this->protocol = protocol;
    address = nullptr;
    session = nullptr;
    socketTCP = nullptr;
}

ImsCommunicator::~ImsCommunicator()
{
    if (address != nullptr) {
        delete address;
    }

    if (session != nullptr) {
        session->socket().close();
        session->abort();
        delete session;
    }

    if (socketTCP != nullptr) {
        socketTCP->close();
        delete socketTCP;
    }
}

bool ImsCommunicator::open() 
{
    if (protocol == ImsRequest::HTTPS) {
        Poco::Net::Context* ctx = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        ctx->enableExtendedCertificateVerification(false);
        session = new Poco::Net::HTTPSClientSession(host, port, ctx);
        session->setTimeout(Poco::Timespan(15, 0));
        return true;

    } else if (protocol == ImsRequest::HTTP) {
        session = new Poco::Net::HTTPClientSession(host, port);
        session->setTimeout(Poco::Timespan(15, 0));
        return true;

    } else if (protocol == ImsRequest::UDP) {

        address = new Poco::Net::SocketAddress(host, port);
        socketTCP = new Poco::Net::StreamSocket(*address);
        socketTCP->setKeepAlive(true);
        socketTCP->setNoDelay(true);
        socketTCP->setOption((int)IPPROTO_TCP, (int)TCP_KEEPIDLE, 20);
        socketTCP->setOption((int)IPPROTO_TCP, (int)TCP_KEEPCNT, 2);
        socketTCP->setOption((int)IPPROTO_TCP, (int)TCP_KEEPINTVL, 10);
        return true;
    }
    return false;
}

void ImsCommunicator::close()
{
    if (protocol == ImsRequest::HTTPS) {
        session->abort();
        session->reset();

    } else if (protocol == ImsRequest::HTTP) {
        session->abort();
        session->reset();

    } else if (protocol == ImsRequest::UDP) {
        socketTCP->close();
    }
}

void ImsCommunicator::addMessageHandler(ImsMessageHandler* handler)
{
    this->handler = handler;
}

void ImsCommunicator::sendRequest(std::shared_ptr<ImsRequest> request, std::shared_ptr<ImsResponse> response)
{
    // -- check there is an acutal request to send
    if (request == nullptr) {
        Poco::Logger::get("ImsCommunicator").error("Request is nullptr : maybe missing login or parameter !", __FILE__, __LINE__);
        response->setStatus(CommandResponse::KO);
        response->setComments("Request is nullptr : maybe missing login or parameter !");
        return;
    }

    // -- FOR DOREMI OR BARCO
    if (protocol == ImsRequest::HTTP || protocol == ImsRequest::HTTPS) {

        // -- check good type of request
        if (request->getProtocol() != ImsRequest::HTTP) {
            Poco::Logger::get("ImsCommunicator").error("Can't send UDP request because communicator is HTTP type !", __FILE__, __LINE__);
            response->setStatus(CommandResponse::KO);
            response->setComments("Can't send UDP request because communicator is HTTP type !");
            return;
        }

        //Poco::Logger::get("ImsCommunicator").debug(request->getPath(), __FILE__, __LINE__);

        // data are XML type
        Poco::Net::HTTPRequest httpRequest(request->getMethod(), request->getPath());
        httpRequest.setContentType(request->getContentType());
        httpRequest.setContentLength(request->getBody().length());

        // -- set cookie for BARCO
        if (protocol == ImsRequest::HTTPS && handler->isLoggedIn()) {
            Poco::Net::NameValueCollection cookies;
            BarcoHandler* temp = (BarcoHandler*)handler;
            cookies.add("SESSIONID_SMS", temp->getSessionId());
            httpRequest.setCookies(cookies);
        }

        try {
            // -- send request and set timestamp
            request->updateTimestamp();
            //Poco::Logger::get("ImsCommunicator").debug("Send request", __FILE__, __LINE__);
            session->sendRequest(httpRequest) << request->getBody() << std::flush;
            //Poco::Logger::get("ImsCommunicator").debug("Request sended", __FILE__, __LINE__);
        } catch (Poco::Net::SSLException &e) {
            Poco::Logger::get("ImsCommunicator").error("No possible to send http request", __FILE__, __LINE__);
            Poco::Logger::get("ImsCommunicator").error(e.displayText(), __FILE__, __LINE__);
            response->setStatus(CommandResponse::KO);
            response->setComments("No possible to send http request !");
            return;
        } catch (std::exception &e) {
            Poco::Logger::get("ImsCommunicator").error("No possible to send http request", __FILE__, __LINE__);
            Poco::Logger::get("ImsCommunicator").error(e.what(), __FILE__, __LINE__);
            response->setStatus(CommandResponse::KO);
            response->setComments("No possible to send http request !");
            return;
        }

        Poco::Net::HTTPResponse responseHttp;
        std::stringstream stringStream;

        try {
            std::istream& stream = session->receiveResponse(responseHttp);
            Poco::StreamCopier::copyStream(stream, stringStream);
        } catch (std::exception &e) {
            Poco::Logger::get("ImsCommunicator").error("No response received", __FILE__, __LINE__);
            Poco::Logger::get("ImsCommunicator").error(e.what(), __FILE__, __LINE__);
            response->setStatus(CommandResponse::KO);
            response->setComments("No response received !");
            return;
        }
        response->updateTimestamp();

        // -- handle response (parsing, collet infos)
        if (stringStream.str() != "") {
            if (responseHttp.getStatus() >= 200 && responseHttp.getStatus() < 300) {
                handler->handleResponse(stringStream.str(), response);
            } else {
                Poco::Logger::get("ImsCommunicator").error("Error at reception of HTTP response", __FILE__, __LINE__);
                std::cout << responseHttp.getStatus() << " : " << responseHttp.getReason() << std::endl;
                response->setStatus(CommandResponse::KO);
                response->setComments("Error at reception of HTTP response !");
            }
        } else {
            Poco::Logger::get("ImsCommunicator").warning("No handlers to parse the response !", __FILE__, __LINE__);
            response->setStatus(CommandResponse::KO);
            response->setComments("No handlers to parse the response !");
        }
    }
    else if (protocol == ImsRequest::UDP) {
        
        if (request->getProtocol() != ImsRequest::UDP) {
            Poco::Logger::get("ImsCommunicator").error("Can't send HTTP request because communicator is UDP type !", __FILE__, __LINE__);
            response->setStatus(CommandResponse::KO);
            response->setComments("Can't send HTTP request because communicator is UDP type !");
            return;
        }

        int nbBytes = socketTCP->sendBytes(request->getPayload(), request->getLength());

        std::stringstream str;
        str << std::hex;
        for (int i = 0; i < request->getLength(); i++) {
            str << std::setw(2) << std::setfill('0') << (int)request->getPayload()[i] << " ";
        }

        if (nbBytes != request->getLength()) {
            Poco::Logger::get("ImsCommunicator").warning("Not all bytes have been sended out. Only " + nbBytes, __FILE__, __LINE__);
            response->setStatus(CommandResponse::KO);
            response->setComments("Not all bytes have been sended out.");
            return;
        }

        // -- end condition
        bool received = false;
        int nbTry = 0;

        // -- get message for reception
        while (!received && nbTry < 200) {

            int available = socketTCP->available();
            if (available > 0) {

                uint8_t buffer[65536];
                int nbBytes = socketTCP->receiveBytes(buffer, available);
                if (nbBytes > 0) {
                    handler->handleResponse(buffer, nbBytes, response);

                    if (response->getStatus() != CommandResponse::UNKNOWN) {
                        received = true;
                    }
                }
                else {
                    Poco::Logger::get("ImsCommunicator").error("No bytes received !", __FILE__, __LINE__);
                }
            }
            else {
                Poco::Logger::get("ImsCommunicator").error("No response received !", __FILE__, __LINE__);
            }

            Timer::crossUsleep(10000);

            nbTry++;
        }

        // -- no reception handler
        if (response->getStatus() == CommandResponse::UNKNOWN) {
            response->setStatus(CommandResponse::KO);
            response->setComments("No response received.");
        }
    }
}