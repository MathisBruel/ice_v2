#include "ScriptState.h"

std::string ScriptState::toXmlString()
{
    std::string xml = "<scripts id_release=\"" + std::to_string(id_release) + "\">";

    std::map<int, std::shared_ptr<ScriptInfoFile>>::iterator it;

    xml += "<features>";
    for (it = scriptFeatureInfo.begin(); it != scriptFeatureInfo.end(); it++) {
        xml += "<server id_server=\"" + std::to_string(it->first) + "\">";
        if (it->second != nullptr) xml += it->second->toXmlString();
        else xml += "NONE";
        xml += "</server>";
    }
    xml += "</features>";

    xml += "<loops>";
    for (it = scriptLoopInfo.begin(); it != scriptLoopInfo.end(); it++) {
        xml += "<server id_server=\"" + std::to_string(it->first) + "\">";
        if (it->second != nullptr) xml += it->second->toXmlString();
        else xml += "NONE";
        xml += "</server>";
    }
    xml += "</loops>";

    xml += "<sass>";
    for (it = scriptSasInfo.begin(); it != scriptSasInfo.end(); it++) {
        xml += "<server id_server=\"" + std::to_string(it->first) + "\">";
        if (it->second != nullptr) xml += it->second->toXmlString();
        else xml += "NONE";
        xml += "</server>";
    }
    xml += "</sass>";

    xml += "</scripts>";
    return xml;
}