#include "render.h"

Render::Render()
    : QObject()
{
    device = "MH-LF";
    isLeft = true;
    zoom = 128;
    ratio = 0;

    posX = posY = posZ = 0;
    minX = minY = minZ = 0;
    maxX = maxY = maxZ = 0;
}
Render::~Render()
{

}

void Render::changeDevice(std::string device) 
{
    this->device = device;
    isLeft = device[3] == 'L';
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

    double ratioF = (double)ratio;

    // -- we determine absolute position of pointage required
    // -- Z is gradually raising with y
    double pointerRequiredX = (double)(maxX - minX) * ratioF + (double)(minX);
    double pointerRequiredY = (double)(maxY - minY) * ratioF + (double)(minY);
    double pointerRequiredZ = (double)(maxZ - minZ) * ratioF + (double)(minZ);

    Poco::Logger::get("MovingHeadProjection").debug("Moving head must point to (X,Y,Z) : (" + 
        std::to_string(pointerRequiredX) + "," + std::to_string(pointerRequiredY) + ","  +std::to_string(pointerRequiredZ) + ")", __FILE__, __LINE__);

    // -- we determine vector from movingHead to pointed position
    double vectorViewX = pointerRequiredX - (double)posX;
    double vectorViewY = pointerRequiredY - (double)posY;
    double vectorViewZ = pointerRequiredZ - (double)posZ;

    // -- normalize vector 
    double length = sqrt(vectorViewX*vectorViewX+vectorViewY*vectorViewY+vectorViewZ*vectorViewZ);
    vectorViewX /= length;
    vectorViewY /= length;
    vectorViewZ /= length;

    double pan = 0.0;
    if (isLeft) {
        pan = std::atan(-vectorViewY/vectorViewX);
    }
    else {
        pan = std::atan(vectorViewY/vectorViewX);
    }
    double tilt = -std::acos(-vectorViewZ);

    // - To validate ! 
    if (!isLeft) {
        pan *= -1;
    }

    // -- transform to degree
    pan *= 180.0 / M_PI;
    tilt *= 180.0 / M_PI;

    std::string beginString = "<command><type>SET_COLOR</type><parameters><device>";
    std::string endString = "";
    endString = "</device><red>255</red><green>0</green><blue>0</blue><amber>0</amber><white>0</white><intensity>255</intensity><zoom>" + std::to_string(zoom) + "</zoom><pan>" + 
    std::to_string(pan) + "</pan><tilt>" + std::to_string(tilt) + "</tilt><force>false</force><lutToApply>false</lutToApply></parameters></command>";

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