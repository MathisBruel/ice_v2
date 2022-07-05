#include "RepoConfiguration.h"

RepoConfiguration::RepoConfiguration(std::string filename)
{
    pathFile = filename;
}
RepoConfiguration::~RepoConfiguration()
{

}

bool RepoConfiguration::load()
{
    // -- check file exists
    Poco::File fileRepo(pathFile);
    if (!fileRepo.exists()) {
        Poco::Logger::get("RepoConfiguration").error("Repo configuration file "+ pathFile + " does not exists !", __FILE__, __LINE__);
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
        Poco::Logger::get("RepoConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
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
        Poco::Logger::get("RepoConfiguration").error("No node configuration in REPO configuration file !", __FILE__, __LINE__);
        doc->release();
        return false;
    }

    // -- parse conf IMS
    Poco::XML::NodeList* childs2 = configuration->childNodes();
    for (int i = 0; i < childs2->length(); i++) {
        Poco::XML::Node* item = childs2->item(i);

        try {
            if (item->nodeName() == "type") {

                if (item->innerText() == "ROOM") {
                    type = ROOM;
                }
                else if (item->innerText() == "DOORWAY") {
                    type = DOORWAY;
                }
            }
            else if (item->nodeName() == "rights") {

                if (item->innerText() == "QC") {
                    rights = QC;
                }
                else {
                    rights = CLASSIC;
                } 
            }
            else if (item->nodeName() == "connector") {

                if (item->innerText() == "FTP") {
                    connector = FTP;
                }
                else {
                    connector = HTTP;
                } 
            }
            else if (item->nodeName() == "port") {
                port = stoi(item->innerText());
            }
            else if (item->nodeName() == "host") {
                host = item->innerText();
            }
            else if (item->nodeName() == "portDownload") {
                portDownload = stoi(item->innerText());
            }
            else if (item->nodeName() == "hostDownload") {
                hostDownload = item->innerText();
            }
            else if (item->nodeName() == "scriptsPath") {
                pathScripts = item->innerText();
            }
            else if (item->nodeName() == "credentials") {
                std::string temp = item->innerText();
                std::string temp2 = Converter::decodeBase64(temp);
                std::string temp3 = Converter::decodeBase64(temp2);
                int posNewLine = temp3.find("\n", 0);
                username = temp3.substr(0, posNewLine);
                password = temp3.substr(posNewLine+1);
            }
            else if (item->nodeName() == "credentialsDownload") {
                std::string temp = item->innerText();
                std::string temp2 = Converter::decodeBase64(temp);
                std::string temp3 = Converter::decodeBase64(temp2);
                int posNewLine = temp3.find("\n", 0);
                usernameDownload = temp3.substr(0, posNewLine);
                passwordDownload = temp3.substr(posNewLine+1);
            }
            else if (item->nodeName() == "cinema") {
                cinema = item->innerText();
            }
        } 
        catch (std::exception &e) {
            Poco::Logger::get("RepoConfiguration").error("A value is not an integer !", __FILE__, __LINE__);
            childs2->release();
            doc->release();
            return false;
        }
    }

    Poco::Logger::get("RepoConfiguration").debug(toString());

    childs2->release();
    doc->release();
    return true;
}

std::string RepoConfiguration::toString()
{
    std::string typeStr = (type == ROOM) ? "ROOM" : (type == DOORWAY) ? "DOORWAY" : "UNKNOWN";

    std::string out = "Type : " + typeStr + "\n";
    out += "Port : " + std::to_string(port) + "\n";
    out += "Host : " + host + "\n";
    out += "Username : " + username + "\n";
    out += "Password : " + password + "\n";
    out += "PAth of scripts : " + pathScripts + "\n";

    return out;
}