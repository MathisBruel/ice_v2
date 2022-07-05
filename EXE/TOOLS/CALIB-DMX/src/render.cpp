#include "render.h"

Render::Render()
    : QObject()
{
    device = "";
    ip = "";
    red = 0;
    green = 0;
    blue = 0;
    amber = 0;
    white = 0;
    pan = 0;
    tilt = 0;
    intensity = 0;
    zoom = 0;
    rgb = true;
}
Render::~Render()
{

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

void Render::run()
{
    Poco::Net::HTTPRequest httpRequest("POST", "/");
    httpRequest.setContentType("application/xml");

    std::string beginString = "<command><type>SET_COLOR</type><parameters><device>";
    
    double redD = red / 256.0;
    double greenD = green / 256.0;
    double blueD = blue / 256.0;
    double whiteD = white / 256.0;

    std::string endString = "";
    if (rgb) {
        endString = "</device><red>" + std::to_string(redD) + "</red><green>" + std::to_string(greenD) + "</green><blue>" + std::to_string(blueD) + 
        "</blue><amber>0</amber><white>0</white><intensity>" + std::to_string(intensity) + "</intensity><zoom>" + std::to_string(zoom) + 
        "</zoom><pan>" + std::to_string(pan) + "</pan><tilt>" + std::to_string(tilt) + "</tilt><force>false</force><lutToApply>false</lutToApply></parameters></command>";
    }
    else {
        endString = "</device><red>0</red><green>0</green><blue>0</blue><amber>" + std::to_string(amber) + "</amber><white> " + std::to_string(whiteD) + 
        "</white><intensity>" + std::to_string(intensity) + "</intensity><zoom>" + std::to_string(zoom) + "</zoom><pan>" + std::to_string(pan) + "</pan><tilt>" + 
        std::to_string(tilt) + "</tilt><force>false</force><lutToApply>false</lutToApply></parameters></command>";
    }

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

    if (rgb) {
        rgb = false;
    }
    else {
        rgb = true;
    }
}