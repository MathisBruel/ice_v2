#include "IMSConfiguration.h"

IMSConfiguration::IMSConfiguration(std::string filename)
{
    pathFile = filename;
    offsetMs2K = offsetMs4K = 0;
}
IMSConfiguration::~IMSConfiguration()
{

}

bool IMSConfiguration::load()
{
    // -- check file exists
    Poco::File fileIms(pathFile);
    if (!fileIms.exists()) {
        Poco::Logger::get("IMSConfiguration").error("IMS configuration file "+ pathFile + " does not exists !", __FILE__, __LINE__);
        return false;
    }

    // -- load file
    std::ifstream in(pathFile);
    Poco::XML::InputSource src(in);

    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parse(&src);
    } 
    catch (std::exception &e) {
        Poco::Logger::get("IMSConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
        in.close();
        return false;
    }
    in.close();

    Poco::XML::Node* configuration = nullptr;
    Poco::XML::NodeList* childs = doc->childNodes();
    for (int h = 0; h < childs->length(); h++) {
        if (childs->item(h)->nodeName() == "configuration") {
            configuration = childs->item(h);
        }
    }
    childs->release();
    if (configuration == nullptr) {
        Poco::Logger::get("IMSConfiguration").error("No node configuration in IMS configuration file !", __FILE__, __LINE__);
        doc->release();
        return false;
    }

    // -- parse conf IMS
    Poco::XML::NodeList* childs2 = configuration->childNodes();
    for (int i = 0; i < childs2->length(); i++) {
        Poco::XML::Node* item = childs2->item(i);

        try {
            if (item->nodeName() == "type") {
                type = item->innerText();
            }
            else if (item->nodeName() == "port") {
                port = stoi(item->innerText());
            }
            else if (item->nodeName() == "ip") {
                ip = item->innerText();
            }
            else if (item->nodeName() == "interface") {
                interfaceName = item->innerText();
            }
            else if (item->nodeName() == "syncRate") {
                syncRate = stoi(item->innerText());
            }
            else if (item->nodeName() == "credentials") {
                std::string temp = item->innerText();
                std::string temp2 = Converter::decodeBase64(temp);
                std::string temp3 = Converter::decodeBase64(temp2);
                int posNewLine = temp3.find("\n", 0);
                username = temp3.substr(0, posNewLine);
                password = temp3.substr(posNewLine+1);
            }
            else if (item->nodeName() == "offsetPlayer2K") {
                offsetMs2K = stoi(item->innerText());
            }
            else if (item->nodeName() == "offsetPlayer4K") {
                offsetMs4K = stoi(item->innerText());
            }
        } 
        catch (std::exception &e) {
            Poco::Logger::get("IMSConfiguration").error("An value is not an integer !", __FILE__, __LINE__);
            doc->release();
            return false;
        }
    }
    childs2->release();
    doc->release();
    return true;
}

std::string IMSConfiguration::toString()
{
    std::string out = "Type : " + type + "\n";
    out += "Port : " + std::to_string(port) + "\n";
    out += "Ip : " + ip + "\n";
    out += "Sync rate : " + std::to_string(syncRate) + "\n";
    out += "Username : " + username + "\n";
    out += "Password : " + password + "\n";

    return out;
}