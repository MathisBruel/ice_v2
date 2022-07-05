#include "IceConfiguration.h"

IceConfiguration::IceConfiguration(std::string filename)
{
    pathFile = filename;
}
IceConfiguration::~IceConfiguration() {

    if (screen != nullptr) {
        delete screen;
    }
    if (frontLeftSeat != nullptr) {
        delete frontLeftSeat;
    }
    if (rearRightSeat != nullptr) {
        delete rearRightSeat;
    }
    if (panels != nullptr) {
        delete[] panels;
    }
    if (movingHeads != nullptr) {
        delete[] movingHeads;
    }
}

bool IceConfiguration::load()
{
    // -- check file exists
    Poco::File fileIms(pathFile);
    if (!fileIms.exists()) {
        Poco::Logger::get("IceConfiguration").error("ICE configuration file " + pathFile + " does not exists !", __FILE__, __LINE__);
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
        Poco::Logger::get("IceConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
        in.close();
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
        Poco::Logger::get("IceConfiguration").error("No node configuration in ICE configuration file !", __FILE__, __LINE__);
        doc->release();
        return false;
    }

    // -- see large
    panels = new PanelConf[50];
    nbPanel = 0;
    movingHeads = new MovingheadConf[10];
    nbMovingHeads = 0;

    // -- parse conf IMS
    childs = configuration->childNodes();
    for (int i = 0; i < childs->length(); i++) {
        Poco::XML::Node* item = childs->item(i);

        if (item->nodeName() == "screen") {

            Poco::XML::NamedNodeMap* attributes2 = item->attributes();
            try {
                screen = new ScreenConf();    
                for (int idxAtt = 0; idxAtt < attributes2->length(); idxAtt++) {
                    Poco::XML::Node* attribute = attributes2->item(idxAtt);

                    if (attribute->nodeName() == "width") {
                        screen->width = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "height") {
                        screen->height = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "offsetX") {
                        screen->offsetX = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "offsetZ") {
                        screen->offsetZ = stoi(attribute->innerText());
                    }
                }
                
            } catch (std::exception &e) {
                Poco::Logger::get("IceConfiguration").error("Exception when parsing screen : a value is not an integer !", __FILE__, __LINE__);
                attributes2->release();
                childs->release();
                doc->release();
                return false;
            }
            attributes2->release();
        }
        else if (item->nodeName() == "seat") {

            SeatConf* tempSeat = new SeatConf();
            bool left = false;
            bool front = false;

            Poco::XML::NamedNodeMap* attributes2 = item->attributes();
            try {
                for (int idxAtt = 0; idxAtt < attributes2->length(); idxAtt++) {
                    Poco::XML::Node* attribute = attributes2->item(idxAtt);

                    if (attribute->nodeName() == "side") {
                        if (attribute->innerText() == "left") {
                            left = true;
                        }
                    }
                    else if (attribute->nodeName() == "depth") {
                        if (attribute->innerText() == "front") {
                            front = true;
                        }
                    }
                    else if (attribute->nodeName() == "offsetX") {
                        tempSeat->offsetX = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "offsetY") {
                        tempSeat->offsetY = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "offsetZ") {
                        tempSeat->offsetZ = stoi(attribute->innerText());
                    }
                }
            } catch (std::exception &e) {
                Poco::Logger::get("IceConfiguration").error("Exception when parsing seats : a value is not an integer !", __FILE__, __LINE__);
                attributes2->release();
                childs->release();
                doc->release();
                return false;
            }
            attributes2->release();

            if (left && front) {
                frontLeftSeat = tempSeat;
            }
            else if (!left && !front) {
                rearRightSeat = tempSeat;
            }
            else {
                std::string a = (left ? "left" : "right");
                std::string b = (front ? "front" : "rear");
                Poco::Logger::get("IceConfiguration").error("Unhandled seat : " + a + ", " + b, __FILE__, __LINE__);
                childs->release();
                doc->release();
                return false;
            }
            
        }
        else if (item->nodeName() == "panel") {

            Poco::XML::NamedNodeMap* attributes2 = item->attributes();
            try {
                for (int idxAtt = 0; idxAtt < attributes2->length(); idxAtt++) {
                    Poco::XML::Node* attribute = attributes2->item(idxAtt);

                    if (attribute->nodeName() == "name") {
                        panels[nbPanel].name = attribute->innerText();
                    }
                    else if (attribute->nodeName() == "offsetX") {
                        panels[nbPanel].offsetX = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "offsetY") {
                        panels[nbPanel].offsetY = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "offsetZ") {
                        panels[nbPanel].offsetZ = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "width") {
                        panels[nbPanel].width = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "height") {
                        panels[nbPanel].height = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "side") {
                        panels[nbPanel].side = SideFromString(attribute->innerText());
                    }
                }
            } catch (std::exception &e) {
                Poco::Logger::get("IceConfiguration").error("Exception when parsing panels : a value is not an integer !", __FILE__, __LINE__);
                attributes2->release();
                childs->release();
                doc->release();
                return false;
            }
            attributes2->release();

            nbPanel++;
        }
        else if (item->nodeName() == "movingHead") {

            Poco::XML::NamedNodeMap* attributes2 = item->attributes();
            try {
                for (int idxAtt = 0; idxAtt < item->attributes()->length(); idxAtt++) {
                    Poco::XML::Node* attribute = item->attributes()->item(idxAtt);

                    if (attribute->nodeName() == "name") {
                        movingHeads[nbMovingHeads].name = attribute->innerText();
                    }
                    else if (attribute->nodeName() == "offsetX") {
                        movingHeads[nbMovingHeads].offsetX = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "offsetY") {
                        movingHeads[nbMovingHeads].offsetY = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "offsetZ") {
                        movingHeads[nbMovingHeads].offsetZ = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "panFullAngle") {
                        movingHeads[nbMovingHeads].panFullAngle = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "tiltFullAngle") {
                        movingHeads[nbMovingHeads].tiltFullAngle = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "biasPan") {
                        movingHeads[nbMovingHeads].biasPan = stod(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "biasTilt") {
                        movingHeads[nbMovingHeads].biasTilt = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "minX") {
                        movingHeads[nbMovingHeads].minX = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "maxX") {
                        movingHeads[nbMovingHeads].maxX = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "minY") {
                        movingHeads[nbMovingHeads].minY = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "maxY") {
                        movingHeads[nbMovingHeads].maxY = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "minZ") {
                        movingHeads[nbMovingHeads].minZ = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "maxZ") {
                        movingHeads[nbMovingHeads].maxZ = stoi(attribute->innerText());
                    }
                    else if (attribute->nodeName() == "depth") {

                        if (attribute->innerText() == "front") {
                            movingHeads[nbMovingHeads].front = true;
                        } else {
                            movingHeads[nbMovingHeads].front = false;
                        }
                    }
                    else if (attribute->nodeName() == "side") {

                        if (attribute->innerText() == "left") {
                            movingHeads[nbMovingHeads].left = true;
                        } else {
                            movingHeads[nbMovingHeads].left = false;
                        }
                    }
                }
            } catch (std::exception &e) {
                Poco::Logger::get("IceConfiguration").error("Exception when parsing movingHeads : a value is not an integer !", __FILE__, __LINE__);
                attributes2->release();
                childs->release();
                doc->release();
                return false;
            }
            attributes2->release();

            nbMovingHeads++;
        }
    }
    childs->release();
    doc->release();
    return true;
}

IceConfiguration::PanelConf* IceConfiguration::getPanel(int index)
{
    if (index < 0 || index >= nbPanel) {
        Poco::Logger::get("IceConfiguration").error("Index is out of bound !", __FILE__, __LINE__);
        return nullptr;
    }

    return &panels[index];
}
IceConfiguration::PanelConf* IceConfiguration::getPanel(std::string name)
{
    for (int i = 0; i < nbPanel; i++) {

        if (panels[i].name == name) {
            return &panels[i];
        }
    }
    return nullptr;
}

IceConfiguration::MovingheadConf* IceConfiguration::getMovingHead(int index)
{
    if (index < 0 || index >= nbMovingHeads) {
        Poco::Logger::get("IceConfiguration").error("Index is out of bound !", __FILE__, __LINE__);
        return nullptr;
    }

    return &movingHeads[index];
}
IceConfiguration::MovingheadConf* IceConfiguration::getMovingHead(std::string name)
{
    for (int i = 0; i < nbMovingHeads; i++) {
        if (movingHeads[i].name == name) {
            return &movingHeads[i];
        }
    }
    return nullptr;
}

IceConfiguration::ProjectionSide IceConfiguration::SideFromString(std::string sideString)
{
    if (sideString == "left") {
        return LEFT;
    } else if (sideString == "right") {
        return RIGHT;
    } else if (sideString == "up") {
        return UP;
    } else {
        return UNKNOW;
    }
}

std::string IceConfiguration::SideToString(IceConfiguration::ProjectionSide side)
{
    if (side == LEFT) {
        return "left";
    } else if (side == RIGHT) {
        return "right";
    } else if (side == UP) {
        return "up";
    } else {
        return "unknown";
    }
}

std::string IceConfiguration::toString()
{
    std::string out = "SCREEN : \n";
    out += std::to_string(screen->width) + ", " + std::to_string(screen->height) + " : " + std::to_string(screen->offsetX) + ", " + std::to_string(screen->offsetZ) + "\n";
    out += "SEATS FRONT LEFT:\n";
    out += std::to_string(frontLeftSeat->offsetX) + ", " + std::to_string(frontLeftSeat->offsetY) + ", " + std::to_string(frontLeftSeat->offsetZ) + "\n";
    out += "SEATS RIGHT REAR:\n";
    out += std::to_string(rearRightSeat->offsetX) + ", " + std::to_string(rearRightSeat->offsetY) + ", " + std::to_string(rearRightSeat->offsetZ) + "\n";
    out += "PANELS : \n";
    for (int i = 0; i < nbPanel; i++) {
        out += panels[i].name + " : " + std::to_string(panels[i].width) + ", " + std::to_string(panels[i].height) + ", " + std::to_string(panels[i].offsetX) + ", " + std::to_string(panels[i].offsetY) + ", " + std::to_string(panels[i].offsetZ) + "\n";
    }
    out += "MOVING HEADS :\n";
    for (int i = 0; i < nbMovingHeads; i++) {
        out += movingHeads[i].name + " : " + std::to_string(movingHeads[i].left) + ", " + std::to_string(movingHeads[i].front) + ", " + std::to_string(movingHeads[i].offsetX) + ", " + std::to_string(movingHeads[i].offsetY) + ", " + std::to_string(movingHeads[i].offsetZ) + "\n";
    }
    out += "\n";
    return out;
}