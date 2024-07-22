#include "commandCentral.h"

CommandCentral::CommandCentral()
{
    nbRetry = 5;
    waitBetweenRetry = 300000;
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    uuid = generator.create().toString();
}
CommandCentral::CommandCentral(CommandCentralType type)
{
    this->type = type;
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    uuid = generator.create().toString();
}
CommandCentral::~CommandCentral() 
{
    parameters.clear();
}

bool CommandCentral::loadFromXmlContent(std::istream& content)
{
    // -- parse
    Poco::XML::DOMParser parser;
    parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACES, false);
	parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, false);
    bool stateWhite = parser.getFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE);
    Poco::Logger::get("Command").debug("State of whitespaces : " + std::to_string(stateWhite), __FILE__, __LINE__);

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
            Poco::XML::NodeList* childs3 = commandParam->childNodes();
            for (int j = 0; j < childs3->length(); j++) {
                Poco::XML::Node* param = childs3->item(j);

                Poco::XML::NodeList * temp = param->childNodes();
                if (temp->length() == 1 && temp->item(0)->nodeType() == 3) {
                    addParameter(param->nodeName(), param->innerText());
                }
                else {
                    Poco::Logger::get("Command").error("Complexe parameters not handled", __FILE__, __LINE__);
                }
                temp->release();
            }
            childs3->release();
        }
    }
    childs2->release();
    doc->release();
    return validateCommand();
}
void CommandCentral::addParameter(std::string key, std::string value)
{
    parameters.insert_or_assign(key, value);
}

int CommandCentral::getIntParameter(std::string key, int defaultValue)
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
std::string CommandCentral::getStringParameter(std::string key, std::string defaultValue)
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
double CommandCentral::getDoubleParameter(std::string key, double defaultValue)
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
bool CommandCentral::getBoolParameter(std::string key, bool defaultValue)
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

bool CommandCentral::validateCommand()
{
    switch (type) {

        case GET_GROUPS:
        case GET_CINEMAS:
        case GET_AUDITORIUMS:
        case GET_SERVERS:
        case GET_FEATURES:
        case GET_RELEASES:
        case GET_SCRIPTS:
        case GET_CPLS:
        case GET_STATE_SERVER:
        case GET_STATE_AUDITORIUM:
        case GET_SCRIPTS_FOR_RELEASE:
        case GET_SCRIPTS_FOR_SERVER:
        case GET_CUT:
        break;

        case INSERT_GROUP:
        case INSERT_CINEMA:
        case INSERT_FEATURE:
        if (getStringParameter("name") == "") {return false;}
        break;

        case INSERT_AUDITORIUM:
        if (getStringParameter("id_cinema") == "" ||
            getStringParameter("name") == "" ||
            getStringParameter("room") == "" ||
            getStringParameter("type_ims") == "" ||
            getStringParameter("ip_ims") == "" ||
            getStringParameter("port_ims") == "" ||
            getStringParameter("user_ims") == "" ||
            getStringParameter("pass_ims") == "") {return false;}
        break;

        case INSERT_SERVER:
        if (getStringParameter("id_auditorium") == "" ||
            getStringParameter("type_server") == "" ||
            getStringParameter("ip") == "") {return false;}
        break;

        case INSERT_SCRIPT:
        if (getStringParameter("name") == "" ||
            getStringParameter("path") == "" ||
            getStringParameter("type") == "" ||
            getStringParameter("version") == "") {return false;}
        break;

        case INSERT_CPL:
        if (getStringParameter("uuid") == "" ||
            getStringParameter("name") == "" ||
            getStringParameter("path_cpl") == "") {return false;}
        break;

        case INSERT_RELEASE:
        if (getStringParameter("name") == "" ||
            getStringParameter("date_in") == "" ||
            getStringParameter("global_pattern") == "") {return false;}
        break;

        case DELETE_GROUP:
        case DELETE_CINEMA:
        case DELETE_AUDITORIUM:
        case DELETE_SERVER:
        case DELETE_SCRIPT:
        case DELETE_CPL:
        case DELETE_FEATURE:
        case DELETE_RELEASE:
        case DELETE_CUT:
        case UPDATE_GROUP:
        case UPDATE_CINEMA:
        case UPDATE_AUDITORIUM:
        case UPDATE_SERVER:
        case UPDATE_SCRIPT:
        case UPDATE_CPL:
        case UPDATE_FEATURE:
        case UPDATE_RELEASE:
        case UPDATE_CUT:
        if (getStringParameter("id") == "") {return false;}
        break;

        // -- ASSOCIATIONS GROUP/CINEMA
        case ADD_CINEMA_TO_GROUP:
        case REMOVE_CINEMA_TO_GROUP:
        if (getStringParameter("id_group") == "" ||
            getStringParameter("id_cinema") == "") {return false;}
        break;

        // -- ASSOCIATIONS RELEASE
        case ADD_GROUP_TO_RELEASE:
        case REMOVE_GROUP_TO_RELEASE:
        if (getStringParameter("id_group") == "" ||
            getStringParameter("id_release") == "") {return false;}
        break;
        case ADD_CINEMA_TO_RELEASE:
        case REMOVE_CINEMA_TO_RELEASE:
        if (getStringParameter("id_release") == "" ||
            getStringParameter("id_cinema") == "") {return false;}
        break;
        case ADD_CPL_TO_RELEASE:
        case REMOVE_CPL_TO_RELEASE:
        if (getStringParameter("id_cpl") == "" ||
            getStringParameter("id_release") == "") {return false;}
        break;
        case ADD_CPL_TO_SCRIPT:
        case REMOVE_CPL_TO_SCRIPT:
        if (getStringParameter("id_cpl") == "" ||
            getStringParameter("id_script") == "") {return false;}
        break;
        break;
        case ADD_SCRIPT_TO_RELEASE:
        case REMOVE_SCRIPT_TO_RELEASE:
        if (getStringParameter("id_script") == "" ||
            getStringParameter("id_release") == "") {return false;}
        break;

        case INSERT_CUT:
        if (getStringParameter("id_release") == "" ||
            getStringParameter("position") == "" ||
            getStringParameter("size") == "") {return false;}
        break;

        case GET_RESULT_SYNCHRO:
        if (getStringParameter("uuid") == "") {return false;}
        break;
    }

    return true;
}

CommandCentral::CommandCentralType CommandCentral::generateFromString(std::string typeStr)
{
    // -- GET
    if (typeStr == "GET_GROUPS") {
        return GET_GROUPS;
    }
    else if (typeStr == "GET_CINEMAS") {
        return GET_CINEMAS;
    }
    else if (typeStr == "GET_AUDITORIUMS") {
        return GET_AUDITORIUMS;
    }
    else if (typeStr == "GET_SERVERS") {
        return GET_SERVERS;
    }
    else if (typeStr == "GET_SCRIPTS") {
        return GET_SCRIPTS;
    }
    else if (typeStr == "GET_CPLS") {
        return GET_CPLS;
    }
    else if (typeStr == "GET_FEATURES") {
        return GET_FEATURES;
    }
    else if (typeStr == "GET_RELEASES") {
        return GET_RELEASES;
    }
    else if (typeStr == "GET_CUT") {
        return GET_CUT;
    }

    // -- INSERT
    else if (typeStr == "INSERT_GROUP") {
        return INSERT_GROUP;
    }
    else if (typeStr == "INSERT_CINEMA") {
        return INSERT_CINEMA;
    }
    else if (typeStr == "INSERT_AUDITORIUM") {
        return INSERT_AUDITORIUM;
    }
    else if (typeStr == "INSERT_SERVER") {
        return INSERT_SERVER;
    }
    else if (typeStr == "INSERT_SCRIPT") {
        return INSERT_SCRIPT;
    }
    else if (typeStr == "INSERT_CPL") {
        return INSERT_CPL;
    }
    else if (typeStr == "INSERT_FEATURE") {
        return INSERT_FEATURE;
    }
    else if (typeStr == "INSERT_RELEASE") {
        return INSERT_RELEASE;
    }

    // -- UPDATE
    else if (typeStr == "UPDATE_GROUP") {
        return UPDATE_GROUP;
    }
    else if (typeStr == "UPDATE_CINEMA") {
        return UPDATE_CINEMA;
    }
    else if (typeStr == "UPDATE_AUDITORIUM") {
        return UPDATE_AUDITORIUM;
    }
    else if (typeStr == "UPDATE_SERVER") {
        return UPDATE_SERVER;
    }
    else if (typeStr == "UPDATE_SCRIPT") {
        return UPDATE_SCRIPT;
    }
    else if (typeStr == "UPDATE_CPL") {
        return UPDATE_CPL;
    }
    else if (typeStr == "UPDATE_FEATURE") {
        return UPDATE_FEATURE;
    }
    else if (typeStr == "UPDATE_RELEASE") {
        return UPDATE_RELEASE;
    }

    // -- DELETE
    else if (typeStr == "DELETE_GROUP") {
        return DELETE_GROUP;
    }
    else if (typeStr == "DELETE_CINEMA") {
        return DELETE_CINEMA;
    }
    else if (typeStr == "DELETE_AUDITORIUM") {
        return DELETE_AUDITORIUM;
    }
    else if (typeStr == "DELETE_SERVER") {
        return DELETE_SERVER;
    }
    else if (typeStr == "DELETE_SCRIPT") {
        return DELETE_SCRIPT;
    }
    else if (typeStr == "DELETE_CPL") {
        return DELETE_CPL;
    }
    else if (typeStr == "DELETE_FEATURE") {
        return DELETE_FEATURE;
    }
    else if (typeStr == "DELETE_RELEASE") {
        return DELETE_RELEASE;
    }

    // -- ASSOCIATIONS GROUP/CINEMA
    else if (typeStr == "ADD_CINEMA_TO_GROUP") {
        return ADD_CINEMA_TO_GROUP;
    }
    else if (typeStr == "REMOVE_CINEMA_TO_GROUP") {
        return REMOVE_CINEMA_TO_GROUP;
    }

    // -- ASSOCIATIONS CPL/SCRIPT/RELEASE
    else if (typeStr == "ADD_GROUP_TO_RELEASE") {
        return ADD_GROUP_TO_RELEASE;
    }
    else if (typeStr == "REMOVE_GROUP_TO_RELEASE") {
        return REMOVE_GROUP_TO_RELEASE;
    }
    else if (typeStr == "ADD_CINEMA_TO_RELEASE") {
        return ADD_CINEMA_TO_RELEASE;
    }
    else if (typeStr == "REMOVE_CINEMA_TO_RELEASE") {
        return REMOVE_CINEMA_TO_RELEASE;
    }
    else if (typeStr == "ADD_CPL_TO_RELEASE") {
        return ADD_CPL_TO_RELEASE;
    }
    else if (typeStr == "REMOVE_CPL_TO_RELEASE") {
        return REMOVE_CPL_TO_RELEASE;
    }
    else if (typeStr == "ADD_CPL_TO_SCRIPT") {
        return ADD_CPL_TO_SCRIPT;
    }
    else if (typeStr == "REMOVE_CPL_TO_SCRIPT") {
        return REMOVE_CPL_TO_SCRIPT;
    }
    else if (typeStr == "ADD_SCRIPT_TO_RELEASE") {
        return ADD_SCRIPT_TO_RELEASE;
    }
    else if (typeStr == "REMOVE_SCRIPT_TO_RELEASE") {
        return REMOVE_SCRIPT_TO_RELEASE;
    }
    // -- CUTS
    else if (typeStr == "INSERT_CUT") {
        return INSERT_CUT;
    }
    else if (typeStr == "UPDATE_CUT") {
        return UPDATE_CUT;
    }
    else if (typeStr == "DELETE_CUT") {
        return DELETE_CUT;
    }

    // -- STATE
    else if (typeStr == "GET_STATE_SERVER") {
        return GET_STATE_SERVER;
    }
    else if (typeStr == "GET_STATE_AUDITORIUM") {
        return GET_STATE_AUDITORIUM;
    }
    else if (typeStr == "GET_RESULT_SYNCHRO") {
        return GET_RESULT_SYNCHRO;
    }
    else if (typeStr == "GET_SCRIPTS_FOR_RELEASE") {
        return GET_SCRIPTS_FOR_RELEASE;
    }


    // -- DOWNLOAD
    else if (typeStr == "GET_SCRIPTS_FOR_SERVER") {
        return GET_SCRIPTS_FOR_SERVER;
    }
    else if (typeStr == "GET_FILE") {
        return GET_FILE;
    }


    // -- State Machine Interaction
    else if (typeStr == "CREATE_CONTENT") {
        return CREATE_CONTENT;
    }
    else if (typeStr == "CREATE_RELEASE") {
        return CREATE_RELEASE;
    }
    else if (typeStr == "RELEASE_CREATED") {
        return RELEASE_CREATED;
    }
    else if (typeStr == "CIS_CREATED") {
        return CIS_CREATED;
    }
    else if (typeStr == "CREATE_CPL") {
        return CREATE_CPL;
    }
    else if (typeStr == "CREATE_SYNCLOOP") {
        return CREATE_SYNCLOOP;
    }
    else if (typeStr == "CPL_CREATED") {
        return CPL_CREATED;
    }
    else if (typeStr == "SYNC_CREATED") {
        return SYNC_CREATED;
    }
    else if (typeStr == "SYNCLOOP_CREATED") {
        return SYNCLOOP_CREATED;
    }
    else if (typeStr == "IMPORT_TO_PROD") {
        return IMPORT_TO_PROD;
    }
    else if (typeStr == "CANCEL") {
        return CANCEL;
    }

    // -- Getters State Machine
    else if (typeStr == "GET_CONTENT") {
        return GET_CONTENT;
    }
    else if (typeStr == "GET_RELEASES_CONTENT") {
        return GET_RELEASES_CONTENT;
    }
    else if (typeStr == "DELETE_RELEASE_CONTENT") {
        return DELETE_RELEASE_CONTENT;
    }
    else if (typeStr == "GET_CPLS_SITE") {
        return GET_CPLS_SITE;
    }

    // -- DEFAULT
    else {
        return UNKNOW_COMMAND;
    }
}

std::string CommandCentral::generateToString(CommandCentral::CommandCentralType type)
{

    // -- GET
    if (type == GET_GROUPS) {
        return "GET_GROUPS";
    }
    else if (type == GET_CINEMAS) {
        return "GET_CINEMAS";
    }
    else if (type == GET_AUDITORIUMS) {
        return "GET_AUDITORIUMS";
    }
    else if (type == GET_SERVERS) {
        return "GET_SERVERS";
    }
    else if (type == GET_SCRIPTS) {
        return "GET_SCRIPTS";
    }
    else if (type == GET_CPLS) {
        return "GET_CPLS";
    }
    else if (type == GET_FEATURES) {
        return "GET_FEATURES";
    }
    else if (type == GET_RELEASES) {
        return "GET_RELEASES";
    }
    else if (type == GET_CUT) {
        return "GET_CUT";
    }

    // -- INSERT
    else if (type == INSERT_GROUP) {
        return "INSERT_GROUP";
    }
    else if (type == INSERT_CINEMA) {
        return "INSERT_CINEMA";
    }
    else if (type == INSERT_AUDITORIUM) {
        return "INSERT_AUDITORIUM";
    }
    else if (type == INSERT_SERVER) {
        return "INSERT_SERVER";
    }
    else if (type == INSERT_SCRIPT) {
        return "INSERT_SCRIPT";
    }
    else if (type == INSERT_CPL) {
        return "INSERT_CPL";
    }
    else if (type == INSERT_FEATURE) {
        return "INSERT_FEATURE";
    }
    else if (type == INSERT_RELEASE) {
        return "INSERT_RELEASE";
    }

    // -- UPDATE
    else if (type == UPDATE_GROUP) {
        return "UPDATE_GROUP";
    }
    else if (type == UPDATE_CINEMA) {
        return "UPDATE_CINEMA";
    }
    else if (type == UPDATE_AUDITORIUM) {
        return "UPDATE_AUDITORIUM";
    }
    else if (type == UPDATE_SERVER) {
        return "UPDATE_SERVER";
    }
    else if (type == UPDATE_SCRIPT) {
        return "UPDATE_SCRIPT";
    }
    else if (type == UPDATE_CPL) {
        return "UPDATE_CPL";
    }
    else if (type == UPDATE_FEATURE) {
        return "UPDATE_FEATURE";
    }
    else if (type == UPDATE_RELEASE) {
        return "UPDATE_RELEASE";
    }

    // -- DELETE
    else if (type == DELETE_GROUP) {
        return "DELETE_GROUP";
    }
    else if (type == DELETE_CINEMA) {
        return "DELETE_CINEMA";
    }
    else if (type == DELETE_AUDITORIUM) {
        return "DELETE_AUDITORIUM";
    }
    else if (type == DELETE_SERVER) {
        return "DELETE_SERVER";
    }
    else if (type == DELETE_SCRIPT) {
        return "DELETE_SCRIPT";
    }
    else if (type == DELETE_CPL) {
        return "DELETE_CPL";
    }
    else if (type == DELETE_FEATURE) {
        return "DELETE_FEATURE";
    }
    else if (type == DELETE_RELEASE) {
        return "DELETE_RELEASE";
    }
    else if (type == INSERT_CUT) {
        return "INSERT_CUT";
    }
    else if (type == UPDATE_CUT) {
        return "UPDATE_CUT";
    }
    else if (type == DELETE_CUT) {
        return "DELETE_CUT";
    }

    // -- ASSOCIATIONS GROUP/CINEMA
    else if (type == ADD_CINEMA_TO_GROUP) {
        return "ADD_CINEMA_TO_GROUP";
    }
    else if (type == REMOVE_CINEMA_TO_GROUP) {
        return "REMOVE_CINEMA_TO_GROUP";
    }
    // -- ASSOCIATIONS RELEASE
    else if (type == ADD_GROUP_TO_RELEASE) {
        return "ADD_GROUP_TO_RELEASE";
    }
    else if (type == REMOVE_GROUP_TO_RELEASE) {
        return "REMOVE_GROUP_TO_RELEASE";
    }
    else if (type == ADD_CINEMA_TO_RELEASE) {
        return "ADD_CINEMA_TO_RELEASE";
    }
    else if (type == REMOVE_CINEMA_TO_RELEASE) {
        return "REMOVE_CINEMA_TO_RELEASE";
    }
    else if (type == ADD_CPL_TO_RELEASE) {
        return "ADD_CPL_TO_RELEASE";
    }
    else if (type == REMOVE_CPL_TO_RELEASE) {
        return "REMOVE_CPL_TO_RELEASE";
    }
    else if (type == ADD_CPL_TO_SCRIPT) {
        return "ADD_CPL_TO_SCRIPT";
    }
    else if (type == REMOVE_CPL_TO_SCRIPT) {
        return "REMOVE_CPL_TO_SCRIPT";
    }
    else if (type == ADD_SCRIPT_TO_RELEASE) {
        return "ADD_SCRIPT_TO_RELEASE";
    }
    else if (type == REMOVE_SCRIPT_TO_RELEASE) {
        return "REMOVE_SCRIPT_TO_RELEASE";
    }

    // -- STATE
    else if (type == GET_STATE_SERVER) {
        return "GET_STATE_SERVER";
    }
    else if (type == GET_STATE_AUDITORIUM) {
        return "GET_STATE_AUDITORIUM";
    }
    else if (type == GET_SCRIPTS_FOR_RELEASE) {
        return "GET_SCRIPTS_FOR_RELEASE";
    }
    else if (type == GET_RESULT_SYNCHRO) {
        return "GET_RESULT_SYNCHRO";
    }

    

    // -- DOWNLOAD
    else if (type == GET_SCRIPTS_FOR_SERVER) {
        return "GET_SCRIPTS_FOR_SERVER";
    }
    else if (type == GET_FILE) {
        return "GET_FILE";
    }

    // -- State Machine Interaction
    else if (type == CREATE_CONTENT) {
        return "CREATE_CONTENT";
    }
    else if (type == CREATE_RELEASE) {
        return "CREATE_RELEASE";
    }
    else if (type == RELEASE_CREATED) {
        return "RELEASE_CREATED";
    }
    else if (type == CIS_CREATED) {
        return "CIS_CREATED";
    }
    else if (type == CREATE_CPL) {
        return "CREATE_CPL";
    }
    else if (type == CREATE_SYNCLOOP) {
        return "CREATE_SYNCLOOP";
    }
    else if (type == CPL_CREATED) {
        return "CPL_CREATED";
    }
    else if (type == SYNC_CREATED) {
        return "SYNC_CREATED";
    }
    else if (type == SYNCLOOP_CREATED) {
        return "SYNCLOOP_CREATED";
    }
    else if (type == IMPORT_TO_PROD) {
        return "IMPORT_TO_PROD";
    }
    else if (type == CANCEL) {
        return "CANCEL";
    }

        // -- Getters State Machine
    else if (type == GET_CONTENT) {
        return "GET_CONTENT";
    }
    else if (type == GET_RELEASES_CONTENT) {
        return "GET_RELEASES_CONTENT";
    }
    else if (type == DELETE_RELEASE_CONTENT) {
        return "DELETE_RELEASE_CONTENT";
    }
    else if (type == GET_CPLS_SITE) {
        return "GET_CPLS_SITE";
    }

    // -- DEFAULT
    else {
        return "UNKNOW_COMMAND";
    }
}