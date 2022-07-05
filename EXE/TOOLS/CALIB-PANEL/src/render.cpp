#include "render.h"

Render::Render(LUT* lut, QWidget* color)
    : QObject()
{
    this->lut = lut;
    this->color = color;
    lastPlayed = "";
    sessionId = "";
    device = "L1";
}
Render::~Render()
{

}
    
void Render::connectIMS(std::string ip, std::string user, std::string pass)
{
    ImsMessageHandler* handler = new DoremiHandler();
    ims = new ImsCommunicator(ip, 10000, ImsRequest::Protocol::HTTP);
    ims->addMessageHandler(handler);
    ims->open();

    std::shared_ptr<ImsRequest> loginRequest = std::shared_ptr<ImsRequest>(handler->createLoginRequest(user, pass));
    std::shared_ptr<ImsResponse> response = std::make_shared<ImsResponse>(ImsResponse("", Command::UNKNOW_COMMAND));
    while (!ims->tryLock()) {Timer::crossUsleep(20);}
    ims->sendRequest(loginRequest, response);
    ims->unlock();
    if (response->getStatus() == ImsResponse::KO) {
        Poco::Logger::get("ImsThread").error("Could not login to IMS !");
    }
}

void Render::connectICE(std::string ip)
{
    sessionId = "";
    sessionICE = new Poco::Net::HTTPClientSession(ip, 8300);
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

    if (title == "IceCalibRGB_TST_S_FR-XX_FR_MOS_2K_GR_20210813_CGR_SMPTE_OV") {
        
        Color ref;

        int pos = (frame - frame%16)/16;
        int offset = (frame%16);
        if (pos == 2) {
            red = lut->getRed()[offset];
            double factorRed = (1.0-red)*(1.0-red);
            red += lut->getGreenOnRed()[0]*factorRed;
            red += lut->getBlueOnRed()[0]*factorRed;
            green = lut->getGreen()[0];
            double factorGreen = (1.0-green)*(1.0-green);
            green += lut->getRedOnGreen()[offset]*factorGreen;
            green += lut->getBlueOnGreen()[0]*factorGreen;
            blue = lut->getBlue()[0];
            double factorBlue = (1.0-blue)*(1.0-blue);
            blue += lut->getRedOnBlue()[offset]*factorBlue;
            blue += lut->getGreenOnBlue()[0]*factorBlue;
        }
        else if (pos == 1) {
            red = lut->getRed()[0];
            double factorRed = (1.0-red)*(1.0-red);
            red += lut->getGreenOnRed()[offset]*factorRed;
            red += lut->getBlueOnRed()[0]*factorRed;
            green = lut->getGreen()[offset];
            double factorGreen = (1.0-green)*(1.0-green);
            green += lut->getRedOnGreen()[0]*factorGreen;
            green += lut->getBlueOnGreen()[0]*factorGreen;
            blue = lut->getBlue()[0];
            double factorBlue = (1.0-blue)*(1.0-blue);
            blue += lut->getRedOnBlue()[0]*factorBlue;
            blue += lut->getGreenOnBlue()[offset]*factorBlue;
        }
        else if (pos == 0) {
            red = lut->getRed()[0];
            double factorRed = (1.0-red)*(1.0-red);
            red += lut->getGreenOnRed()[0]*factorRed;
            red += lut->getBlueOnRed()[offset]*factorRed;
            green = lut->getGreen()[0];
            double factorGreen = (1.0-green)*(1.0-green);
            green += lut->getRedOnGreen()[0]*factorGreen;
            green += lut->getBlueOnGreen()[offset]*factorGreen;
            blue = lut->getBlue()[offset];
            double factorBlue = (1.0-blue)*(1.0-blue);
            blue += lut->getRedOnBlue()[0]*factorBlue;
            blue += lut->getGreenOnBlue()[0]*factorBlue;
        }

        std::string toPlay = "RGB:" + std::to_string(pos) + ":" + std::to_string(offset);

        if (toPlay != lastPlayed) {
            // -- play
            int redI = red*255.0+0.5;
            int greenI = green*255.0+0.5;
            int blueI = blue*255.0+0.5;
            std::string colorString = "background-color: rgb(" + std::to_string(redI) + ", " + std::to_string(greenI) + ", " + std::to_string(blueI) + ");";
            color->setStyleSheet(QString::fromStdString(colorString));
            playColor(redI, greenI, blueI);
            lastPlayed = toPlay;
        }
    }

    else if (title == "IceCalibRG_TST_S_FR-XX_FR_MOS_2K_GR_20210813_CGR_SMPTE_OV") {
        

        int offsetG = (frame - frame%16)/16;
        int offsetR = (frame%16);

        Color ref;
        ref.setRGB(offsetR*17.0/255.0, offsetG*17.0/255.0, 0);

        red = lut->getRed()[offsetR];
        double factorRed = (1.0-red)*(1.0-red);
        red += lut->getGreenOnRed()[offsetG]*factorRed;
        red += lut->getBlueOnRed()[0]*factorRed;

        green = lut->getGreen()[offsetG];
        double factorGreen = (1.0-green)*(1.0-green);
        green += lut->getRedOnGreen()[offsetR]*factorGreen;
        green += lut->getBlueOnGreen()[0]*factorGreen;

        blue = lut->getBlue()[0];
        double factorBlue = (1.0-blue)*(1.0-blue);
        blue += lut->getRedOnBlue()[offsetR]*factorBlue;
        blue += lut->getGreenOnBlue()[0]*factorBlue;

        Color dst;
        dst.setRGB(red, green, blue);

        double redF = dst.getRed();
        double greenF = dst.getGreen();
        double blueF = dst.getBlue();

        std::string toPlay = "RG:" + std::to_string(offsetR) + ":" + std::to_string(offsetG);

        if (toPlay != lastPlayed) {
            // -- play
            int redI = redF*255.0+0.5;
            int greenI = greenF*255.0+0.5;
            int blueI = blueF*255.0+0.5;
            std::string colorString = "background-color: rgb(" + std::to_string(redI) + ", " + std::to_string(greenI) + ", " + std::to_string(blueI) + ");";
            color->setStyleSheet(QString::fromStdString(colorString));
            playColor(redI, greenI, blueI);
            lastPlayed = toPlay;
        }
    }

    else if (title == "IceCalibRB_TST_S_FR-XX_FR_MOS_2K_GR_20210813_CGR_SMPTE_OV") {
        
        int offsetR = (frame - frame%16)/16;
        int offsetB = (frame%16);

        Color ref;
        ref.setRGB(offsetR*17.0/255.0, 0, offsetB*17.0/255.0);

        red = lut->getRed()[offsetR];
        double factorRed = (1.0-red)*(1.0-red);
        red += lut->getGreenOnRed()[0]*factorRed;
        red += lut->getBlueOnRed()[offsetB]*factorRed;

        green = lut->getGreen()[0];
        double factorGreen = (1.0-green)*(1.0-green);
        green += lut->getRedOnGreen()[offsetR]*factorGreen;
        green += lut->getBlueOnGreen()[offsetB]*factorGreen;

        blue = lut->getBlue()[offsetB];
        double factorBlue = (1.0-blue)*(1.0-blue);
        blue += lut->getRedOnBlue()[offsetR]*factorBlue;
        blue += lut->getGreenOnBlue()[0]*factorBlue;

        Color dst;
        dst.setRGB(red, green, blue);

        double redF = dst.getRed();
        double greenF = dst.getGreen();
        double blueF = dst.getBlue();

        std::string toPlay = "BR:" + std::to_string(offsetR) + ":" + std::to_string(offsetB);

        if (toPlay != lastPlayed) {
            // -- play
            int redI = redF*255.0+0.5;
            int greenI = greenF*255.0+0.5;
            int blueI = blueF*255.0+0.5;
            std::string colorString = "background-color: rgb(" + std::to_string(redI) + ", " + std::to_string(greenI) + ", " + std::to_string(blueI) + ");";
            color->setStyleSheet(QString::fromStdString(colorString));
            playColor(redI, greenI, blueI);
            lastPlayed = toPlay;
        }
    }

    else if (title == "IceCalibGB_TST_S_FR-XX_FR_MOS_2K_GR_20210813_CGR_SMPTE_OV") {
        
        int offsetB = (frame - frame%16)/16;
        int offsetG = (frame%16);

        Color ref;
        ref.setRGB(0, offsetG*17.0/255.0, offsetB*17.0/255.0);

        red = lut->getRed()[0];
        double factorRed = (1.0-red)*(1.0-red);
        red += lut->getGreenOnRed()[offsetG]*factorRed;
        red += lut->getBlueOnRed()[offsetB]*factorRed;

        green = lut->getGreen()[offsetG];
        double factorGreen = (1.0-green)*(1.0-green);
        green += lut->getRedOnGreen()[0]*factorGreen;
        green += lut->getBlueOnGreen()[offsetB]*factorGreen;
        
        blue = lut->getBlue()[offsetB];
        double factorBlue = (1.0-blue)*(1.0-blue);
        blue += lut->getRedOnBlue()[0]*factorBlue;
        blue += lut->getGreenOnBlue()[offsetG]*factorBlue;

        Color dst;
        dst.setRGB(red, green, blue);

        double redF = dst.getRed();
        double greenF = dst.getGreen();
        double blueF = dst.getBlue();

        std::string toPlay = "GB:" + std::to_string(offsetB) + ":" + std::to_string(offsetG);

        if (toPlay != lastPlayed) {
            // -- play
            int redI = redF*255.0+0.5;
            int greenI = greenF*255.0+0.5;
            int blueI = blueF*255.0+0.5;
            std::string colorString = "background-color: rgb(" + std::to_string(redI) + ", " + std::to_string(greenI) + ", " + std::to_string(blueI) + ");";
            color->setStyleSheet(QString::fromStdString(colorString));
            playColor(redI, greenI, blueI);
            lastPlayed = toPlay;
        }
    }
    
    else if (title == "IceCalibLUM_TST_S_FR-XX_FR_MOS_2K_GR_20210813_CGR_SMPTE_OV") {
        
        int value = frame*25.5;

        red = lut->getRedFinalPoly()[value];
        green = lut->getGreenFinalPoly()[value];
        blue = lut->getBlueFinalPoly()[value];

        Color* colorRef = new Color();
        colorRef->setRGB(red, green, blue);
        Poco::Logger::get("Render").debug("Render LUM1 : " + colorRef->toString() + ", " + colorRef->toHSLString(), __FILE__, __LINE__);

        double factorRed = (1.0-red)*(1.0-red);
        double factorGreen = (1.0-green)*(1.0-green);
        double factorBlue = (1.0-blue)*(1.0-blue);

        double biasRed = lut->getGreenOnRedFinalPoly()[value]*factorRed + lut->getBlueOnRedFinalPoly()[value]*factorRed;
        double biasGreen = lut->getRedOnGreenFinalPoly()[value]*factorGreen + lut->getBlueOnGreenFinalPoly()[value]*factorGreen;
        double biasBlue = lut->getRedOnBlueFinalPoly()[value]*factorBlue + lut->getGreenOnBlueFinalPoly()[value]*factorBlue;

        red += biasRed;
        green += biasGreen;
        blue += biasBlue;

        Poco::Logger::get("Render").debug("Bias : " + std::to_string(biasRed) + ", " + std::to_string(biasGreen) + ", " + std::to_string(biasBlue), __FILE__, __LINE__);

        colorRef->setRGB(red, green, blue);
        
        double redF = colorRef->getRed();   
        double greenF = colorRef->getGreen();
        double blueF = colorRef->getBlue();

        std::string toPlay = "LUM:" + std::to_string(value);

        if (toPlay != lastPlayed) {
            // -- play
            int redI = redF*255.0+0.5;
            int greenI = greenF*255.0+0.5;
            int blueI = blueF*255.0+0.5;
            std::string colorString = "background-color: rgb(" + std::to_string(redI) + ", " + std::to_string(greenI) + ", " + std::to_string(blueI) + ");";
            color->setStyleSheet(QString::fromStdString(colorString));
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
            color->setStyleSheet(QString::fromStdString(colorString));
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
        std::string body = beginString + device + endString;
        httpRequest.setContentLength(body.length());
        sessionICE->sendRequest(httpRequest) << body << std::flush;

    } catch (std::exception &e) {
        Poco::Logger::get("Render").error("No possible to send http request", __FILE__, __LINE__);
    }
}