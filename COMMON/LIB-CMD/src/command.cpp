#include "command.h"

Command::Command()
{
    dispatch = UNKNOW;
    nbRetry = 5;
    waitBetweenRetry = 300000;
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    uuid = generator.create().toString();
}
Command::Command(CommandType type)
{
    this->type = type;
    setDispatch();
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    uuid = generator.create().toString();
}
Command::~Command() 
{
    parameters.clear();
}

void Command::setDispatch()
{
    switch (type) {
        case SET_COLOR:
        case SET_IMAGE:
        case SET_POINTER:
        case SET_PAR_SCENE_ON:
        case SET_PAR_SCENE_OFF:
        case SET_PAR_AMBIANCE_ON:
        case SET_PAR_AMBIANCE_OFF:
            dispatch = KINET; 
        break;

        case PLAY_TEMPLATE_IMAGE:
            dispatch = PLAYER;
        break;
        case GET_CPL_CONTENT:
        case GET_CPL_CONTENT_NAME:
            nbRetry = 5;
            waitBetweenRetry = 500000;
            dispatch = IMS;
        break;

        case CHANGE_PRIORITY_SCRIPT:
        case SYNCHRONIZE_SCRIPT:
        case GET_SYNC_CONTENT:
            dispatch = REPOSITORY;
        break;

        case LIST_DEVICES:
        case GET_MODE:
        case STATUS_IMS:
        case GET_CPL_INFOS:
        case GET_DOWNLOADED_SCRIPTS:
        case SWITCH_LUT:
        case UNFORCE_DEVICE:
        case GET_CURRENT_SYNC:
        case UNFORCE_ALL_DEVICES:
        case STOP_SCRIPT:
        case GET_CURRENT_PROJECTION:
        case SWITCH_PROJECTION:
        case LAUNCH_SCRIPT:
        case SWITCH_MODE:
        case GET_CONFIGURATION:
        case GET_SCENES:
        case PLAY_SCENE:
        case CREATE_UPDATE_SCENE:
        case REMOVE_SCENE:
            dispatch = CONTEXT;
        break;

        case UNKNOW_COMMAND:
            dispatch = UNKNOW;
        break;
    }
}

bool Command::loadFromXmlContent(std::istream& content)
{
    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    Poco::XML::InputSource src(content);
    
    try {
        doc = parser.parse(&src);
    } catch (std::exception &e) {
        Poco::Logger::get("Command").error("Exception when parsing XML file !", __FILE__, __LINE__);
    }

    Poco::XML::Node* command = nullptr;
    Poco::XML::NodeList* childs = doc->childNodes();
    for (int h = 0; h < childs->length(); h++) {
        if (childs->item(h)->nodeName() == "command") {
            command = childs->item(h);
        }
    }
    childs->release();
    if (command == nullptr) {
        Poco::Logger::get("Command").error("No node command found !", __FILE__, __LINE__);
        doc->release();
        return false;
    }

    // -- parse definition of command
    Poco::XML::NodeList* childs2 = command->childNodes();
    for (int i = 0; i < childs2->length(); i++) {

        Poco::XML::Node* commandParam = childs2->item(i);
        if (commandParam->nodeName() == "type") {
            
            type = generateFromString(commandParam->innerText());
            if (type == UNKNOW_COMMAND) {
                Poco::Logger::get("Command").warning("Unknown command " + commandParam->innerText(), __FILE__, __LINE__);
                childs2->release();
                doc->release();
                return false;
            }
        }

        else if (commandParam->nodeName() == "parameters") {

            // -- parse parameters
            // -- parse parameters
            Poco::XML::NodeList* childs3 = commandParam->childNodes();
            if (childs3 != nullptr) {

                for (int j = 0; j < childs3->length(); j++) {
                    Poco::XML::Node* param = childs3->item(j);

                    if (param != nullptr) {

                        Poco::XML::NodeList * temp = param->childNodes();
                        
                        if (temp != nullptr) {

                            if (temp->length() == 0) {
                                if (param->hasAttributes()) {
                                    ComplexParam paramC;
                                    paramC.load(param);
                                    complex.insert_or_assign(param->nodeName(), paramC);
                                }
                                else {
                                    // -- ignore
                                }
                            }
                            else {
                                if (temp->length() == 1 && temp->item(0)->nodeType() == 3) {
                                    if (param->hasAttributes()) {
                                        ComplexParam paramC;
                                        paramC.load(param);
                                        complex.insert_or_assign(param->nodeName(), paramC);
                                    }
                                    else {
                                        addParameter(param->nodeName(), param->innerText());
                                    }
                                }
                                else {
                                    ComplexParam paramC;
                                    paramC.load(param);
                                    complex.insert_or_assign(param->nodeName(), paramC);
                                }
                            }
                            temp->release();
                        }
                    }
                }
            }
            childs3->release();
        }
    }
    childs2->release();
    doc->release();
    setDispatch();
    return validateCommand();
}
void Command::addParameter(std::string key, std::string value)
{
    parameters.insert_or_assign(key, value);
}

int Command::getIntParameter(std::string key, int defaultValue)
{
    std::map<std::string, std::string>::iterator it = parameters.find(key);
    if (it != parameters.end()) {
        try {
            int value = std::stoi(it->second);
            return value;
        }
        catch (std::exception &e) {
            Poco::Logger::get("Command").error("Cast parameters " + key + " to int failed !", __FILE__, __LINE__);
            return defaultValue;
        }
    }
    else {
        Poco::Logger::get("Command").debug("Parameter " + key + " can't be found !", __FILE__, __LINE__);
        return defaultValue;
    }
}
std::string Command::getStringParameter(std::string key, std::string defaultValue)
{
    std::map<std::string, std::string>::iterator it = parameters.find(key);
    if (it != parameters.end()) {
        try {
            return it->second;
        }
        catch (std::exception &e) {
            Poco::Logger::get("Command").error("Cast parameters " + key + " to string failed !", __FILE__, __LINE__);
            return defaultValue;
        }
    }
    else {
        Poco::Logger::get("Command").debug("Parameter " + key + " can't be found !", __FILE__, __LINE__);
        return defaultValue;
    }
}
double Command::getDoubleParameter(std::string key, double defaultValue)
{
    std::map<std::string, std::string>::iterator it = parameters.find(key);
    if (it != parameters.end()) {
        try {
            double value = std::stod(it->second);
            return value;
        }
        catch (std::exception &e) {
            Poco::Logger::get("Command").error("Cast parameters " + key + " to double failed !", __FILE__, __LINE__);
            return defaultValue;
        }
    }
    else {
        Poco::Logger::get("Command").debug("Parameter " + key + " can't be found !", __FILE__, __LINE__);
        return defaultValue;
    }
}
bool Command::getBoolParameter(std::string key, bool defaultValue)
{
    std::map<std::string, std::string>::iterator it = parameters.find(key);
    if (it != parameters.end()) {
        if (it->second == "true") {
            return true;
        }
        else if (it->second == "false") {
            return false;
        }
        else {
            return defaultValue;
        }
    }
    else {
        Poco::Logger::get("Command").debug("Parameter " + key + " can't be found !", __FILE__, __LINE__);
        return defaultValue;
    }
}

ComplexParam* Command::getComplexParam(std::string name)
{
    std::map<std::string, ComplexParam>::iterator it = complex.find(name);
    if (it == complex.end()) {
        return nullptr;
    }
    else {
        return &(it->second);
    }
}

bool Command::validateCommand()
{
    switch (type) {

        // -- NO PARAMETERS
        case LIST_DEVICES:
        case GET_MODE:
        case STATUS_IMS:
        case GET_CPL_INFOS:
        case UNFORCE_ALL_DEVICES:
        // -- filtering is done client side
        case GET_DOWNLOADED_SCRIPTS:
        case GET_CURRENT_SYNC:
        case STOP_SCRIPT:
        case GET_CURRENT_PROJECTION:
        case SET_PAR_SCENE_ON:
        case SET_PAR_SCENE_OFF:
        case SET_PAR_AMBIANCE_ON:
        case SET_PAR_AMBIANCE_OFF:
        case GET_CONFIGURATION:
        case GET_SCENES:
            break;

        case PLAY_SCENE:
        case REMOVE_SCENE:
        case CREATE_UPDATE_SCENE:
            if (getStringParameter("scene") == "") {
                return false;
            }
            break;

        case SWITCH_PROJECTION:
            if (getStringParameter("projection") == "") {
                return false;
            }
            break;

        // -- minimum filter parameter
        case GET_CPL_CONTENT:
            if (getStringParameter("cplId") == "") {
                return false;
            }
            break;

        case GET_SYNC_CONTENT:
            if (getStringParameter("syncId") == "") {
                return false;
            }
            break;

        case GET_CPL_CONTENT_NAME:
            if (getStringParameter("search") == "") {
                return false;
            }
            break;

        case CHANGE_PRIORITY_SCRIPT:
            if (getStringParameter("scriptName") == "") {
                return false;
            }
            break;
            if (getStringParameter("priority") == "") {
                return false;
            }
            break;
        case SYNCHRONIZE_SCRIPT:
            if (getStringParameter("cpl") == "") {
                return false;
            }
            break;

        // -- intensity, white, and zoom can be given too
        case SET_COLOR:
            if (getStringParameter("device") == "") {
                return false;
            }
            else if (getIntParameter("red") == -1) {
                return false;
            }
            else if (getIntParameter("green") == -1) {
                return false;
            }
            else if (getIntParameter("blue") == -1) {
                return false;
            }
            break;

        case SET_IMAGE:
            if (getStringParameter("device") == "") {
                return false;
            }
            else if (getStringParameter("bytes") == "") {
                return false;
            }
            break;

        case SET_POINTER:
            if (getStringParameter("device") == "") {
                return false;
            }
            else if (getDoubleParameter("x") == -1) {
                return false;
            }
            else if (getDoubleParameter("y") == -1) {
                return false;
            }
            else if (getIntParameter("red") == -1) {
                return false;
            }
            else if (getIntParameter("green") == -1) {
                return false;
            }
            else if (getIntParameter("blue") == -1) {
                return false;
            }
            break;

        case UNFORCE_DEVICE:
            if (getStringParameter("device") == "") {
                return false;
            }
            break;

        case SWITCH_LUT:
            if (getStringParameter("device") == "") {
                return false;
            }
            if (getStringParameter("lut") == "") {
                return false;
            }
            break;

        case PLAY_TEMPLATE_IMAGE:
            if (getStringParameter("templateVersion") == "") {
                return false;
            }
            if (getStringParameter("bytes") == "") {
                return false;
            }
            if (getStringParameter("format") == "") {
                return false;
            }
            if (getStringParameter("projectionType") == "") {
                return false;
            }
            break;

        case LAUNCH_SCRIPT:
            if (getStringParameter("script") == "") {
                return false;
            }
            break;
        case SWITCH_MODE:
            if (getStringParameter("mode") == "") {
                return false;
            }
            break;

        case UNKNOW_COMMAND:
            return false;
            break;
    }

    return true;
}

Command::CommandType Command::generateFromString(std::string typeStr)
{

    // -- KINET DISCOVER
    if (typeStr == "LIST_DEVICES") {
        return LIST_DEVICES;
    }

    // -- CONTEXT
    else if (typeStr == "UNFORCE_DEVICE") {
        return UNFORCE_DEVICE;
    }
    else if (typeStr == "UNFORCE_ALL_DEVICES") {
        return UNFORCE_ALL_DEVICES;
    }
    else if (typeStr == "GET_CURRENT_SYNC") {
        return GET_CURRENT_SYNC;
    }
    else if (typeStr == "GET_CURRENT_PROJECTION") {
        return GET_CURRENT_PROJECTION;
    }
    else if (typeStr == "SWITCH_PROJECTION") {
        return SWITCH_PROJECTION;
    }
    else if (typeStr == "LAUNCH_SCRIPT") {
        return LAUNCH_SCRIPT;
    }
    else if (typeStr == "SWITCH_MODE") {
        return SWITCH_MODE;
    }
    else if (typeStr == "GET_MODE") {
        return GET_MODE;
    }
    else if (typeStr == "GET_CONFIGURATION") {
        return GET_CONFIGURATION;
    }
    else if (typeStr == "GET_SCENES") {
        return GET_SCENES;
    }
    else if (typeStr == "PLAY_SCENE") {
        return PLAY_SCENE;
    }
    else if (typeStr == "CREATE_UPDATE_SCENE") {
        return CREATE_UPDATE_SCENE;
    }
    else if (typeStr == "REMOVE_SCENE") {
        return REMOVE_SCENE;
    }
    else if (typeStr == "STOP_SCRIPT") {
        return STOP_SCRIPT;
    }

    // KINET SET
    else if (typeStr == "SET_COLOR") {
        return SET_COLOR;
    }
    else if (typeStr == "SET_IMAGE") {
        return SET_IMAGE;
    }
    else if (typeStr == "SET_POINTER") {
        return SET_POINTER;
    }
    else if (typeStr == "SET_PAR_SCENE_OFF") {
        return SET_PAR_SCENE_OFF;
    }
    else if (typeStr == "SET_PAR_SCENE_ON") {
        return SET_PAR_SCENE_ON;
    }
    else if (typeStr == "SET_PAR_AMBIANCE_OFF") {
        return SET_PAR_AMBIANCE_OFF;
    }
    else if (typeStr == "SET_PAR_AMBIANCE_ON") {
        return SET_PAR_AMBIANCE_ON;
    } 

    // -- LUTS
    else if (typeStr == "SWITCH_LUT") {
        return SWITCH_LUT;
    }

    // -- PLAYER
    else if (typeStr == "PLAY_TEMPLATE_IMAGE") {
        return PLAY_TEMPLATE_IMAGE;
    }

    // -- IMS
    else if (typeStr == "STATUS_IMS") {
        return STATUS_IMS;
    }
    else if (typeStr == "GET_CPL_INFOS") {
        return GET_CPL_INFOS;
    }
    else if (typeStr == "GET_CPL_CONTENT") {
        return GET_CPL_CONTENT;
    }
    else if (typeStr == "GET_CPL_CONTENT_NAME") {
        return GET_CPL_CONTENT_NAME;
    }

    // -- REPOSITORY
    else if (typeStr == "GET_SYNC_CONTENT") {
        return GET_SYNC_CONTENT;
    }
    else if (typeStr == "GET_DOWNLOADED_SCRIPTS") {
        return GET_DOWNLOADED_SCRIPTS;
    }

    else if (typeStr == "CHANGE_PRIORITY_SCRIPT") {
        return CHANGE_PRIORITY_SCRIPT;
    }
    else if (typeStr == "SYNCHRONIZE_SCRIPT") {
        return SYNCHRONIZE_SCRIPT;
    }

    // -- DEFAULT
    else {
        return UNKNOW_COMMAND;
    }
}

std::string Command::generateToString(Command::CommandType type)
{

    // -- KINET DISCOVER
    if (type == LIST_DEVICES) {
        return "LIST_DEVICES";
    }

    // -- CONTEXT
    else if (type == UNFORCE_DEVICE) {
        return "UNFORCE_DEVICE";
    }
    else if (type == UNFORCE_ALL_DEVICES) {
        return "UNFORCE_ALL_DEVICES";
    }
    else if (type == GET_CURRENT_SYNC) {
        return "GET_CURRENT_SYNC";
    }
    else if (type == GET_CURRENT_PROJECTION) {
        return "GET_CURRENT_PROJECTION";
    }
    else if (type == SWITCH_PROJECTION) {
        return "SWITCH_PROJECTION";
    }
    else if (type == LAUNCH_SCRIPT) {
        return "LAUNCH_SCRIPT";
    }
    else if (type == SWITCH_MODE) {
        return "SWITCH_MODE";
    }
    else if (type == GET_MODE) {
        return "GET_MODE";
    }
    else if (type == GET_CONFIGURATION) {
        return "GET_CONFIGURATION";
    }
    else if (type == GET_SCENES) {
        return "GET_SCENES";
    }
    else if (type == PLAY_SCENE) {
        return "PLAY_SCENE";
    }
    else if (type == CREATE_UPDATE_SCENE) {
        return "CREATE_UPDATE_SCENE";
    }
    else if (type == REMOVE_SCENE) {
        return "REMOVE_SCENE";
    }
    else if (type == STOP_SCRIPT) {
        return "STOP_SCRIPT";
    }

    // KINET SET
    else if (type == SET_COLOR) {
        return "SET_COLOR";
    }
    else if (type == SET_IMAGE) {
        return "SET_IMAGE";
    }
    else if (type == SET_POINTER) {
        return "SET_POINTER";
    }
    else if (type == SET_PAR_SCENE_OFF) {
        return "SET_PAR_SCENE_OFF";
    }
    else if (type == SET_PAR_SCENE_ON) {
        return "SET_PAR_SCENE_ON";
    }
    else if (type == SET_PAR_AMBIANCE_OFF) {
        return "SET_PAR_AMBIANCE_OFF";
    }
    else if (type == SET_PAR_AMBIANCE_ON) {
        return "SET_PAR_AMBIANCE_ON";
    }

    // -- LUTS
    else if (type == SWITCH_LUT) {
        return "SWITCH_LUT";
    }

    // -- PLAYER
    else if (type == PLAY_TEMPLATE_IMAGE) {
        return "PLAY_TEMPLATE_IMAGE";
    }

    // -- IMS
    else if (type == STATUS_IMS) {
        return "STATUS_IMS";
    }
    else if (type == GET_CPL_INFOS) {
        return "GET_CPL_INFOS";
    }
    else if (type == GET_CPL_CONTENT) {
        return "GET_CPL_CONTENT";
    }

    // -- REPOSITORY
    else if (type == GET_SYNC_CONTENT) {
        return "GET_SYNC_CONTENT";
    }
    else if (type == GET_DOWNLOADED_SCRIPTS) {
        return "GET_DOWNLOADED_SCRIPTS";
    }

    else if (type == CHANGE_PRIORITY_SCRIPT) {
        return "CHANGE_PRIORITY_SCRIPT";
    }
    else if (type == SYNCHRONIZE_SCRIPT) {
        return "SYNCHRONIZE_SCRIPT";
    }

    // -- DEFAULT
    else {
        return "UNKNOW_COMMAND";
    }
}