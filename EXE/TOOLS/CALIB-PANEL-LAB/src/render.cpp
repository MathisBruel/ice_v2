#include "render.h"

Render::Render(LabCalib* lab)
    : QObject()
{
    this->lab = lab;
    lastPlayed = "";
    sessionId = "";

    imsConf = new IMSConfiguration("IMSConfiguration.xml");
    Poco::Logger::get("Render").debug("Loading IMSConfiguration.xml", __FILE__, __LINE__);
    if (!imsConf->load()) {
        Poco::Logger::get("Render").error("Error when loading IMSConfiguration.xml !", __FILE__, __LINE__);
    }

    kinetConf = new KinetConfiguration("KinetConfiguration.xml");
    Poco::Logger::get("Render").debug("Loading KinetConfiguration.xml", __FILE__, __LINE__);
    if (!kinetConf->load()) {
        Poco::Logger::get("Render").error("Error when loading KinetConfiguration.xml !", __FILE__, __LINE__);
    }

    // -- LOAD IMS
    ImsRequest::Protocol protocol = ImsRequest::Protocol::HTTP;
    ImsMessageHandler* msgHandler = msgHandler = new DoremiHandler();
    ims = new ImsCommunicator(imsConf->getIpIms(), imsConf->getPort(), protocol);
    ims->addMessageHandler(msgHandler);
    ims->open();

    sessionICE = new Poco::Net::HTTPClientSession("192.168.33.12", 8300);
    sessionICE->setTimeout(Poco::Timespan(10, 0));

    Poco::Net::HTTPRequest httpRequest("POST", "/login");
    httpRequest.setContentType("application/xml");
    std::string body = "<command><username>admin</username><password>ice17180</password></command>";
    httpRequest.setContentLength(body.length());

    try {
        // -- send request and set timestamp
        sessionICE->sendRequest(httpRequest) << body << std::flush;
    } catch (std::exception &e) {
        Poco::Logger::get("Render").error("No possible to send http request", __FILE__, __LINE__);
    }

    Poco::Net::HTTPResponse responseHttp;

    try {
        std::istream& stream = sessionICE->receiveResponse(responseHttp);
        std::vector<Poco::Net::HTTPCookie> cookies;
        responseHttp.getCookies(cookies);
        std::vector<Poco::Net::HTTPCookie>::iterator cookieIt = cookies.begin();
        for (; cookieIt != cookies.end(); cookieIt++) {

            if (cookieIt->getName() == "sessionId") {
                sessionId = cookieIt->getValue();
            }
            Poco::Logger::get("Render").debug("Cookie key: " + cookieIt->getName() + " !", __FILE__, __LINE__);
            Poco::Logger::get("Render").debug("Cookie value: " + cookieIt->getValue() + " !", __FILE__, __LINE__);
        }
    } catch (std::exception &e) {
        Poco::Logger::get("Render").error("No response received", __FILE__, __LINE__);
    }
}
Render::~Render()
{

}
    
void Render::init()
{
    ImsMessageHandler* handler = ims->getHandler();
    std::shared_ptr<ImsRequest> loginRequest = std::shared_ptr<ImsRequest>(handler->createLoginRequest(imsConf->getUsername(), imsConf->getPassword()));
    std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::UNKNOW_COMMAND));
    while (!ims->tryLock()) {Timer::crossUsleep(20);}
    ims->sendRequest(loginRequest, response);
    ims->unlock();
    if (response->getStatus() == ImsResponse::KO) {
        Poco::Logger::get("ImsThread").error("Could not login to IMS !");
    }
}

void Render::resetEmission()
{
    lastPlayed = "";
}

void Render::run()
{
    ImsMessageHandler* handler = ims->getHandler();
    std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::STATUS_IMS));

    if (ims->getProtocol() != ImsRequest::UDP) {
        std::shared_ptr<ImsRequest> playStatusRequest = std::shared_ptr<ImsRequest>(handler->createGetPlayerStatusRequest());
        while (!ims->tryLock()) {Timer::crossUsleep(20);}
        ims->sendRequest(playStatusRequest, response);
        ims->unlock();
    }
    else {
        // -- STATE
        std::shared_ptr<ImsRequest> playStateRequest = std::shared_ptr<ImsRequest>(handler->createGetStatusItemRequest(ChristieHandler::PLAYBACK_STATE));
        while (!ims->tryLock()) {Timer::crossUsleep(20);}
        ims->sendRequest(playStateRequest, response);
        ims->unlock();

        // -- TIMECODE
        std::shared_ptr<ImsRequest> playTimecodeRequest = std::shared_ptr<ImsRequest>(handler->createGetStatusItemRequest(ChristieHandler::PLAYBACK_TIMECODE));
        while (!ims->tryLock()) {Timer::crossUsleep(20);}
        ims->sendRequest(playTimecodeRequest, response);
        ims->unlock();

        // -- CPLID
        std::shared_ptr<ImsRequest> playCplIdRequest = std::shared_ptr<ImsRequest>(handler->createGetStatusItemRequest(ChristieHandler::PLAYBACK_CPLID));
        while (!ims->tryLock()) {Timer::crossUsleep(20);}
        ims->sendRequest(playCplIdRequest, response);
        ims->unlock();

        // -- CPLNAME
        std::shared_ptr<ImsRequest> playCplNameRequest = std::shared_ptr<ImsRequest>(handler->createGetStatusItemRequest(ChristieHandler::PLAYBACK_CPLNAME));
        while (!ims->tryLock()) {Timer::crossUsleep(20);}
        ims->sendRequest(playCplNameRequest, response);
        ims->unlock();
    }

    if (response->getStatus() == ImsResponse::KO) {
        // -- error no new response
        Poco::Logger::get("Render").warning("Response to get show status not received in time. Extrapolation needed !");
    }
    else {
        int frame = response->getPlayerStatus()->getTimeCodeMs()*0.024+0.5;
        handlePlay(response->getPlayerStatus()->getCplTitle(), frame);
    }
}

void Render::handlePlay(std::string title, int frame)
{
    double red = 0.0;
    double green = 0.0;
    double blue = 0.0;

    if (title == "LAB_CALIBRATION") {
        
        Color* refColor = new Color();
        if (frame = 0) {refColor->setRGB(0, 0, 0);}
        else if (frame = 1) {refColor->setRGB(17, 17, 17);}
        else if (frame = 2) {refColor->setRGB(27, 27, 27);}
        else if (frame = 3) {refColor->setRGB(38, 38, 38);}
        else if (frame = 4) {refColor->setRGB(48, 48, 48);}
        else if (frame = 5) {refColor->setRGB(59, 59, 59);}
        else if (frame = 6) {refColor->setRGB(71, 71, 71);}
        else if (frame = 7) {refColor->setRGB(82, 82, 82);}
        else if (frame = 8) {refColor->setRGB(94, 94, 94);}
        else if (frame = 9) {refColor->setRGB(106, 106, 106);}
        else if (frame = 10) {refColor->setRGB(119, 119, 119);}
        else if (frame = 11) {refColor->setRGB(132, 132, 132);}
        else if (frame = 12) {refColor->setRGB(145, 145, 145);}
        else if (frame = 13) {refColor->setRGB(158, 158, 158);}
        else if (frame = 14) {refColor->setRGB(171, 171, 171);}
        else if (frame = 15) {refColor->setRGB(185, 185, 185);}
        else if (frame = 16) {refColor->setRGB(198, 198, 198);}
        else if (frame = 17) {refColor->setRGB(212, 212, 212);}
        else if (frame = 18) {refColor->setRGB(226, 226, 226);}
        else if (frame = 19) {refColor->setRGB(241, 241, 241);}
        else if (frame = 20) {refColor->setRGB(255, 255, 255);}

        refColor->convertRGBToXYZ();
        refColor->convertXYZToLAB();

        double finalL = lab->getDataDstL()[frame];
        double finalA = refColor->getA() * lab->getFactorA() + lab->getBiasA();
        double finalB = refColor->getB() * lab->getFactorB() + lab->getBiasB();

        delete refColor;

        Color dst;
        dst.setLAB(finalL, finalA, finalB);
        dst.convertLABToXYZ();
        dst.convertXYZToRGB();

        double redF = dst.getRed();
        double greenF = dst.getGreen();
        double blueF = dst.getBlue();

        std::string toPlay = "L:" + std::to_string(frame);

        if (toPlay != lastPlayed) {
            // -- play
            int redI = redF*255.0+0.5;
            int greenI = greenF*255.0+0.5;
            int blueI = blueF*255.0+0.5;
            std::string colorString = "background-color: rgb(" + std::to_string(redI) + ", " + std::to_string(greenI) + ", " + std::to_string(blueI) + ");";
            playColor(redI, greenI, blueI);
            lastPlayed = toPlay;
        }
    }

    else {
        if (lastPlayed != "") {
            int redI = 0;
            int greenI = 0;
            int blueI = 0;
            std::string colorString = "background-color: rgb(" + std::to_string(redI) + ", " + std::to_string(greenI) + ", " + std::to_string(blueI) + ");";
            playColor(redI, greenI, blueI);
        }

        lastPlayed = "";
    }
}

void Render::playColor(int red, int green, int blue)
{
    Poco::Net::HTTPRequest httpRequest("POST", "/");
    httpRequest.setContentType("application/xml");

    std::string beginString = "<command><type>SET_COLOR</type><parameters><device>";
    std::string endString = "</device><red>" + std::to_string(red) + "</red><green>" + std::to_string(green) + "</green><blue>" + std::to_string(blue) + "</blue><force>false</force><lutToApply>false</lutToApply></parameters></command>";

    std::string color = "RGB (" + std::to_string(red) + ", " + std::to_string(green) + ", " + std::to_string(blue) + ")";
    Poco::Logger::get("Render").error("Playing " + color, __FILE__, __LINE__);

    Poco::Net::NameValueCollection cookies;
    cookies.add("sessionId", sessionId);
    httpRequest.setCookies(cookies);

    try {
        // -- send request and set timestamp
        for (int i = 0; i < kinetConf->getNbPanels(); i++) {
            std::string body = beginString + kinetConf->getPanel(i)->name + endString;
            httpRequest.setContentLength(body.length());
            sessionICE->sendRequest(httpRequest) << body << std::flush;
        }
    } catch (std::exception &e) {
        Poco::Logger::get("Render").error("No possible to send http request", __FILE__, __LINE__);
    }
}