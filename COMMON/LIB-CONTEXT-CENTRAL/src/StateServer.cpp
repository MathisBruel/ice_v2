#include "StateServer.h"

StateServer::StateServer(int id, bool ims)
{
    this->id = id;
    this->ims = ims;
    answering = false;
    lastTimeCheck = -1;
}
StateServer::~StateServer()
{

}

void StateServer::setAnswering(bool answering, std::string comments)
{
    answering = answering;
    this->comments = comments;
    devicesInErrors.clear();
    lastTimeCheck = Poco::Timestamp().epochMicroseconds();
}
void StateServer::addDeviceInError(std::string device)
{
    devicesInErrors.push_back(device);
}


std::string StateServer::toXmlString()
{
    std::string xml = "<state";
    xml += " id=\"" + std::to_string(id) + "\"";
    if (ims) xml += " type=\"IMS\"";
    else xml += " type=\"SERVER\"";
    xml += " lastTimeCheck=\"" + std::to_string(lastTimeCheck) + "\"";
    if (answering) xml += " answering=\"true\"";
    else {
        xml += " answering=\"false\"";
        xml += " comments=\"" + comments + "\"";
    }

    if (!ims) {
        xml += ">";
        for (std::string device : devicesInErrors) {
            xml += "<error name=\"" + device + "\" />";
        }
        xml += "</state>";
    }
    else {
        xml += " />";
    }

    return xml;
}