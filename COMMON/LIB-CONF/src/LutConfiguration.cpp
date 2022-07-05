#include "LutConfiguration.h"

LutConfiguration::LutConfiguration(std::string filename)
{
    pathFile = filename;
}
LutConfiguration::~LutConfiguration() {

    std::map<std::string, LUT*>::iterator it;
    for (it = luts.begin(); it != luts.end(); it++) {
        delete it->second;
    }

    std::map<std::string, Calibrer*>::iterator itC;
    for (itC = calibs.begin(); itC != calibs.end(); itC++) {
        delete itC->second;
    }
}

bool LutConfiguration::load()
{
    // -- check file exists
    Poco::File fileIms(pathFile);
    if (!fileIms.exists()) {
        Poco::Logger::get("LutConfiguration").error("LUT configuration file " + pathFile + " does not exists !", __FILE__, __LINE__);
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
    } catch (std::exception &e) {
        Poco::Logger::get("LutConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
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
        Poco::Logger::get("LutConfiguration").error("No node configuration in LUT configuration file !", __FILE__, __LINE__);
        doc->release();
        return false;
    }

    // -- parse conf IMS
    childs = configuration->childNodes();
    for (int i = 0; i < childs->length(); i++) {
        Poco::XML::Node* item = childs->item(i);

        if (item->nodeName() == "lut") {
            
            LUT* lut = new LUT();
            std::string name ="";
            std::string path ="";

            Poco::XML::NamedNodeMap* attributes = item->attributes();
            for (int idxAtt = 0; idxAtt < attributes->length(); idxAtt++) {
                Poco::XML::Node* attribute = attributes->item(idxAtt);

                if (attribute->nodeName() == "name") {
                    name = attribute->innerText();
                }
                else if (attribute->nodeName() == "filename") {
                    path = attribute->innerText();
                }
            }
            attributes->release();

            Poco::File treeFile(path);
            if (!treeFile.exists()) {
                Poco::Logger::get("LutConfiguration").error("Lut file " + path + " does not exists !", __FILE__, __LINE__);
                childs->release();
                doc->release();
                return false;
            }

            if (!lut->open(path)) {
                Poco::Logger::get("LutConfiguration").error("Error when loading lut file " + path + " !", __FILE__, __LINE__);
                childs->release();
                doc->release();
                return false;
            }

            // -- long loading : do it or not ? 
            lut->precomputeAll();

            if (lut != nullptr && name != "") {
                luts.insert_or_assign(name, lut);
            }
        }
    
        else if (item->nodeName() == "calib") {
            
            Calibrer* calib = new Calibrer();
            std::string name ="";
            std::string path ="";

            Poco::XML::NamedNodeMap* attributes = item->attributes();
            for (int idxAtt = 0; idxAtt < attributes->length(); idxAtt++) {
                Poco::XML::Node* attribute = attributes->item(idxAtt);

                if (attribute->nodeName() == "name") {
                    name = attribute->innerText();
                }
                else if (attribute->nodeName() == "filename") {
                    path = attribute->innerText();
                }
            }
            attributes->release();

            Poco::File treeFile(path);
            if (!treeFile.exists()) {
                Poco::Logger::get("LutConfiguration").error("Calib file " + path + " does not exists !", __FILE__, __LINE__);
                childs->release();
                doc->release();
                return false;
            }

            if (!calib->open(path)) {
                Poco::Logger::get("LutConfiguration").error("Error when loading lut file " + path + " !", __FILE__, __LINE__);
                childs->release();
                doc->release();
                return false;
            }

            if (calib != nullptr && name != "") {
                calibs.insert_or_assign(name, calib);
            }
        }
    }
    
    childs->release();
    doc->release();
    return true;
}

LUT* LutConfiguration::getLutWithName(std::string name)
{
    std::map<std::string, LUT*>::iterator it;
    for (it = luts.begin(); it != luts.end() ; it++) {
        if (it->first == name) {
            return it->second;
        }
    }
    return nullptr;
}

Calibrer* LutConfiguration::getCalibWithName(std::string name)
{
    std::map<std::string, Calibrer*>::iterator it;
    for (it = calibs.begin(); it != calibs.end() ; it++) {
        if (it->first == name) {
            return it->second;
        }
    }
    return nullptr;
}

std::string LutConfiguration::toString()
{
    std::string out = "";
    std::map<std::string, LUT*>::iterator it;
    for (it = luts.begin(); it != luts.end() ; it++) {
        out += it->first + " loaded ! \n";
    }
    std::map<std::string, Calibrer*>::iterator itC;
    for (itC = calibs.begin(); itC != calibs.end() ; itC++) {
        out += itC->first + " loaded ! \n";
    }
    return out;
}