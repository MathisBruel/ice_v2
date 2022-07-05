#include "KinetConfiguration.h"

KinetConfiguration::KinetConfiguration(std::string filename)
{
    pathFile = filename;
    panels = nullptr;
    dmxs = nullptr;
}
KinetConfiguration::~KinetConfiguration()
{
    if (panels != nullptr) {

        for (int i = 0; i < nbPanels; i++) {
            delete[] panels[i].lutPriority;
        }
        delete[] panels;
    }
    if (dmxs != nullptr) {

        for (int i = 0; i < nbDmxControler; i++) {

            for (int j = 0; j < dmxs[i].nbLights; j++) {
                delete[] dmxs[i].lights[j].calib;
            }
            delete[] dmxs[i].lights;
        }
        delete[] dmxs;
    }
}

bool KinetConfiguration::load()
{
    // -- check file exists
    Poco::File fileKinet(pathFile);
    if (!fileKinet.exists()) {
        Poco::Logger::get("KinetConfiguration").error("Kinet configuration file " + pathFile + " does not exists !", __FILE__, __LINE__);
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
        Poco::Logger::get("KinetConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
        in.close();
        doc->release();
        return false;
    }
    in.close();

    nbPanels = 0;
    nbDmxControler = 0;

    Poco::XML::Node* configuration = nullptr;
    Poco::XML::NodeList* childs = doc->childNodes();
    for (int h = 0; h < childs->length(); h++) {
        if (childs->item(h)->nodeName() == "configuration") {
            configuration = childs->item(h);
        }
    }
    childs->release();
    if (configuration == nullptr) {
        Poco::Logger::get("KinetConfiguration").error("No node configuration in kinet configuration file !", __FILE__, __LINE__);
        doc->release();
        return false;
    }

    Poco::XML::NamedNodeMap* attributes = configuration->attributes();
    for (int idxAtt = 0; idxAtt < attributes->length(); idxAtt++) {
        Poco::XML::Node* attribute = attributes->item(idxAtt);

        if (attribute->nodeName() == "interface") {
            interfaceKinet = attribute->innerText();
        }
        else if (attribute->nodeName() == "ipICE") {
            ipServer = attribute->innerText();
        }
    }
    attributes->release();

    // -- start getting number of elements
    Poco::XML::NodeList* childs2 = configuration->childNodes();
    for (int i = 0; i < childs2->length(); i++) {

        if (childs2->item(i)->nodeName() == "panel") {
            nbPanels++;
        }
        else if (childs2->item(i)->nodeName() == "DMX") {
            nbDmxControler++;
        }
    }

    panels = new PanelConf[nbPanels];
    dmxs = new DmxControlerConf[nbDmxControler];

    // -- get values
    int offsetPanel = 0;
    int offsetDmx = 0;
    for (int i = 0; i < childs2->length(); i++) {

        Poco::XML::Node* device = childs2->item(i);
        if (device->nodeName() == "panel") {

            Poco::XML::NamedNodeMap* attributes2 = device->attributes();
            try {
                // -- parsing attributes
                for (int idxAtt = 0; idxAtt < attributes2->length(); idxAtt++) {
                    Poco::XML::Node* attribute = attributes2->item(idxAtt);
                    if (attribute->nodeName() == "name") {
                        panels[offsetPanel].name = attribute->innerText();
                    }
                    else if (attribute->nodeName() == "ip") {
                        panels[offsetPanel].ip = attribute->innerText();
                    }
                    else if (attribute->nodeName() == "serial") {
                        panels[offsetPanel].serial = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "sizePixel") {
                        panels[offsetPanel].sizePixel = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "nbPixels") {
                        panels[offsetPanel].nbPixels = stoi(attribute->innerText());
                    }
                }
            } catch (std::exception &e) {
                Poco::Logger::get("KinetConfiguration").error("Exception when parsing panel : a value is not an integer !", __FILE__, __LINE__);
                attributes2->release();
                childs2->release();
                doc->release();
                return false;
            }
            attributes2->release();

            // -- try to find lut associated
            panels[offsetPanel].nbLut = 0;
            Poco::XML::NodeList* childs3 = device->childNodes();
            for (int j = 0; j < childs3->length(); j++) {
                if (childs3->item(j)->nodeName() == "lut") {
                    panels[offsetPanel].nbLut++;
                }
            }

            panels[offsetPanel].lutPriority = new std::string[panels[offsetPanel].nbLut];
            int offsetLut = 0;

            // -- fill lut
            for (int j = 0; j < childs3->length(); j++) {
                if (childs3->item(j)->nodeName() == "lut") {
                    panels[offsetPanel].lutPriority[offsetLut] = childs3->item(j)->innerText();
                    offsetLut++;
                }
            }
            childs3->release();

            offsetPanel++;
        }
        else if (device->nodeName() == "DMX") {
            
            Poco::XML::NamedNodeMap* attributes2 = device->attributes();
            try {
                // -- parsing attributes
                for (int idxAtt = 0; idxAtt < attributes2->length(); idxAtt++) {
                    Poco::XML::Node* attribute = attributes2->item(idxAtt);
                    if (attribute->nodeName() == "name") {
                        dmxs[offsetDmx].name = attribute->innerText();
                    }
                    else if (attribute->nodeName() == "ip") {
                        dmxs[offsetDmx].ip = attribute->innerText();
                    }
                    else if (attribute->nodeName() == "serial") {
                        dmxs[offsetDmx].serial = stoi(attribute->innerText());
                    }
                }
            } catch (std::exception &e) {
                Poco::Logger::get("KinetConfiguration").error("Exception when parsing DMX : a value is not an integer !", __FILE__, __LINE__);
                attributes2->release();
                childs2->release();
                doc->release();
                return false;
            }
            attributes2->release();

            dmxs[offsetDmx].nbLights = 0;

            // -- try to find lut associated
            Poco::XML::NodeList* childs3 = device->childNodes();
            for (int j = 0; j < childs3->length(); j++) {
                if (childs3->item(j)->nodeName() == "device") {
                    dmxs[offsetDmx].nbLights++;
                }
            }

            dmxs[offsetDmx].lights = new DmxDeviceConf[dmxs[offsetDmx].nbLights];
            int offsetDmxDevice = 0;
            dmxs[offsetDmx].lights[offsetDmxDevice].lumens = 0;
            
            // -- read all dmx devices
            for (int j = 0; j < childs3->length(); j++) {
                
                Poco::XML::Node* dmx = childs3->item(j);
                if (dmx->nodeName() == "device") {

                    Poco::XML::NamedNodeMap* attributes2 = dmx->attributes();
                    try {
                        // -- read dmx devices attributes
                        for (int idxAtt = 0; idxAtt < attributes2->length(); idxAtt++) {
                            Poco::XML::Node* attribute = attributes2->item(idxAtt);

                            if (attribute->nodeName() == "name") {
                                dmxs[offsetDmx].lights[offsetDmxDevice].name = attribute->innerText();
                            }
                            else if (attribute->nodeName() == "type") {

                                dmxs[offsetDmx].lights[offsetDmxDevice].type = TypeFromString(attribute->innerText());
                            }
                            else if (attribute->nodeName() == "category") {

                                dmxs[offsetDmx].lights[offsetDmxDevice].category = attribute->innerText();
                            }
                            else if (attribute->nodeName() == "dmxOffset") {
                                dmxs[offsetDmx].lights[offsetDmxDevice].dmxOffset = stoi(attribute->innerText());
                            }
                            else if (attribute->nodeName() == "formatBuffer") {
                                dmxs[offsetDmx].lights[offsetDmxDevice].formatBuffer = attribute->innerText();
                            }
                            else if (attribute->nodeName() == "zoom") {
                                dmxs[offsetDmx].lights[offsetDmxDevice].defaultZoom = stoi(attribute->innerText());
                            }
                            else if (attribute->nodeName() == "lumens") {
                                dmxs[offsetDmx].lights[offsetDmxDevice].lumens = stoi(attribute->innerText());
                            }
                        }
                    } catch (std::exception &e) {
                        Poco::Logger::get("KinetConfiguration").error("Exception when parsing device : a value is not an integer !", __FILE__, __LINE__);
                        attributes2->release();
                        childs3->release();
                        childs2->release();
                        doc->release();
                        return false;
                    }
                    attributes2->release();

                    dmxs[offsetDmx].lights[offsetDmxDevice].nbCalib = 0;
                    dmxs[offsetDmx].lights[offsetDmxDevice].refCalib = "";

                    // -- try to find lut associated
                    Poco::XML::NodeList* childs4 = dmx->childNodes();
                    for (int k = 0; k < childs4->length(); k++) {
                        if (childs4->item(k)->nodeName() == "calib") {
                            dmxs[offsetDmx].lights[offsetDmxDevice].nbCalib++;
                        }
                    }

                    dmxs[offsetDmx].lights[offsetDmxDevice].calib = new std::string[dmxs[offsetDmx].lights[offsetDmxDevice].nbCalib];
                    int offsetLut = 0;

                    // -- fill lut and ref lut
                    for (int k = 0; k < childs4->length(); k++) {
                        if (childs4->item(k)->nodeName() == "calib") {
                            dmxs[offsetDmx].lights[offsetDmxDevice].calib[offsetLut] = childs4->item(k)->innerText();
                            offsetLut++;
                        }
                        else if (childs4->item(k)->nodeName() == "ref") {
                            dmxs[offsetDmx].lights[offsetDmxDevice].refCalib = childs4->item(k)->innerText();
                        }
                    }
                    childs4->release();

                    offsetDmxDevice++;
                }
            }

            childs3->release();
            offsetDmx++;
        }
    }

    childs2->release();
    doc->release();
    return true;
}

KinetConfiguration::PanelConf* KinetConfiguration::getPanel(int index)
{
    if (index < 0 || index >= nbPanels) {
        Poco::Logger::get("KinetConfiguration").error("Index out of bounds !", __FILE__, __LINE__);
        return nullptr;
    }
    return &panels[index];
}

KinetConfiguration::PanelConf* KinetConfiguration::getPanel(std::string name)
{
    for (int i = 0 ; i < nbPanels; i++) {
        if (panels[i].name == name) {
            return &panels[i];
        }
    }
    return nullptr;
}

KinetConfiguration::DmxControlerConf* KinetConfiguration::getDmxControler(int index)
{
    if (index < 0 || index >= nbDmxControler) {
        Poco::Logger::get("KinetConfiguration").error("Index out of bounds !", __FILE__, __LINE__);
        return nullptr;
    }
    return &dmxs[index];
}

KinetConfiguration::DmxControlerConf* KinetConfiguration::getDmxControler(std::string name)
{
    for (int i = 0 ; i < nbDmxControler; i++) {
        if (dmxs[i].name == name) {
            return &dmxs[i];
        }
    }
    return nullptr;
}

KinetConfiguration::DmxType KinetConfiguration::TypeFromString(std::string typeString)
{
    if (typeString == "BACKLIGHT") {
        return KinetConfiguration::BACKLIGHT;
    } else if (typeString == "PAR") {
        return KinetConfiguration::PAR;
    } else if (typeString == "MOVINGHEAD") {
        return KinetConfiguration::MOVINGHEAD;
    } else {
        return KinetConfiguration::UNKNOW;
    }
}

std::string KinetConfiguration::toString()
{
    std::string out = "Interface : " + interfaceKinet + "\n";
    
    out += "PANELS :\n";
    for (int i = 0; i < nbPanels; i++) {
        out += panels[i].name + " : " + panels[i].ip + ", " + std::to_string(panels[i].serial) + ", " + std::to_string(panels[i].sizePixel) + ", " + std::to_string(panels[i].nbPixels) + "\n";
        out += "LUTS : \n";
        for (int j = 0; j < panels[i].nbLut; j++) {
            out += panels[i].lutPriority[j] + "\n";
        }
        out += "\n";
    }

    for (int i = 0; i < nbDmxControler; i++) {
        out += dmxs[i].name + " : " + dmxs[i].ip + ", " + std::to_string(dmxs[i].serial) + "\n";
        out += "DEVICES : \n";
        for (int j = 0; j < dmxs[i].nbLights; j++) {
            out += dmxs[i].lights[j].name + " : " + std::to_string(dmxs[i].lights[j].dmxOffset) + ", " + dmxs[i].lights[j].formatBuffer + "\n";

            out += "LUTS : \n";
            for (int k = 0; k < dmxs[i].lights[j].nbCalib; k++) {
                out += dmxs[i].lights[j].calib[k] + "\n";
            }
            out += "\n";
        }
        out += "\n";
    }

    return out;
}