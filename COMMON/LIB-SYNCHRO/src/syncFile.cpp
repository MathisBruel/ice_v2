#include "syncFile.h"

SyncFile::SyncFile(std::string pathFile)
    : Synchronizable(pathFile)
{

}

SyncFile::~SyncFile()
{
    
}

bool SyncFile::load()
{
    Poco::File file(pathFile);
    if (!file.exists()) {
        Poco::Logger::get("SyncFile").error("File " + pathFile + " does not exists ! ", __FILE__, __LINE__);
        return false;
    }

    std::ifstream stream;
    Poco::Dynamic::Var result;

    try {
        stream.open(pathFile);
    
    } catch (std::exception &e) {
        Poco::Logger::get("SyncFile").error("Exception when opening Sync file !", __FILE__, __LINE__);
        return false;
    }

    try {
        Poco::JSON::Parser parser;
        result = parser.parse(stream);
    
    } catch (std::exception &e) {
        Poco::Logger::get("SyncFile").error("Exception when parsing Sync file !", __FILE__, __LINE__);
        Poco::Logger::get("SyncFile").error(e.what(), __FILE__, __LINE__);
        stream.close();
        return false;
    }

    stream.close();

    try {
        // -- we close now
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        // -- parse generic information
        version = object->getValue<std::string>("version");
        type = object->getValue<std::string>("type");
        sha1 = object->getValue<std::string>("itemSha1");
        cplId = object->getValue<std::string>("cplUuid");
        startDelay = object->getValue<int>("startDelay");
        entryPoint = object->getValue<int>("entryPoint");
        cplDuration = object->getValue<int>("cplDuration");
        cplTitle = object->getValue<std::string>("title");
     
        // -- parameter projection is not mandatory
        try {

            if (object->has("projection")) {
                std::string projection = object->getValue<std::string>("projection");
                if (projection == "NO_FIT") {
                    typeProjection = PanelProjection::NO_FIT;
                }
                else if (projection == "FIT_X") {
                    typeProjection = PanelProjection::FIT_X;
                }
                else if (projection == "FIT_Y") {
                    typeProjection = PanelProjection::FIT_Y;
                }
                else if (projection == "FIT_X_PROP_Y") {
                    typeProjection = PanelProjection::FIT_X_PROP_Y;
                }
                else if (projection == "FIT_Y_PROP_X") {
                    typeProjection = PanelProjection::FIT_Y_PROP_X;
                }
                else if (projection == "FIT_X_FIT_Y") {
                    typeProjection = PanelProjection::FIT_X_FIT_Y;
                }
                else {
                    typeProjection = PanelProjection::UNKNOWN;
                }
            }

        } catch (std::exception &e) {
            typeProjection = PanelProjection::UNKNOWN;
        }

        cplEditRate = object->getValue<std::string>("cplEditRate");
        int posSep = cplEditRate.find(" ", 0);
        if (posSep == std::string::npos) {
            Poco::Logger::get("SyncFile").error("Sync file has member cplEditRate badly formatted !");
            return false;
        }
        speedRate = std::stod(cplEditRate.substr(0, posSep)) / std::stod(cplEditRate.substr(posSep+1));

        // -- parse reels information
        Poco::JSON::Array::Ptr reelsJson = object->getArray("reels");
        if (reelsJson.isNull()) {
            Poco::Logger::get("SyncFile").error("Sync file has no member reels !");
            return false;
        }

        for (int i = 0; i < reelsJson->size(); i++) {
            Poco::Dynamic::Var resultReel = reelsJson->get(i);
            Poco::JSON::Object::Ptr objectReel = resultReel.extract<Poco::JSON::Object::Ptr>();
            Synchronizable::Reel reel;
            reel.duration = objectReel->getValue<int>("duration");
            reel.editRate = objectReel->getValue<std::string>("editRate");
            int posSep = cplEditRate.find(" ", 0);
            if (posSep == std::string::npos) {
                Poco::Logger::get("SyncFile").error("Sync file has member editRate badly formatted !");
                return false;
            }

            if (objectReel->has("id")) {
                reel.id = objectReel->getValue<std::string>("id");
            }
            else {
                reel.id = "";
            }
            if (objectReel->has("keyId")) {
                reel.keyId = objectReel->getValue<std::string>("keyId");
            }
            if (objectReel->has("hash")) {
                reel.hash = objectReel->getValue<std::string>("hash");
            }
            reel.speedRate = std::stod(cplEditRate.substr(0, posSep)) / std::stod(cplEditRate.substr(posSep+1));
            reels.push_back(reel);
        }

        // -- parse cutscenes
        if (object->has("cuts")) {
            Poco::JSON::Object::Ptr cutsJson = object->getObject("cuts");
            if (!cutsJson.isNull()) {
                std::string cplCutRefId = cutsJson->getValue<std::string>("cplIdRef");
                std::string cplCutRefTitle = cutsJson->getValue<std::string>("cplTitleRef");
                cuts = new CutScenes(cplId, cplTitle, cplCutRefId, cplCutRefTitle);

                Poco::JSON::Array::Ptr arrayCutsJson = cutsJson->getArray("scenes");
                if (arrayCutsJson.isNull()) {
                    Poco::Logger::get("SyncFile").error("Sync file has cuts but no scene cuts defined !");
                    return false;
                }
                for (int i = 0; i < arrayCutsJson->size(); i++) {
                    Poco::Dynamic::Var resultScenes = arrayCutsJson->get(i);
                    Poco::JSON::Object::Ptr objectScene = resultScenes.extract<Poco::JSON::Object::Ptr>();
                    cuts->addCut(objectScene->getValue<int>("startFrame"), objectScene->getValue<int>("endFrame"), objectScene->getValue<std::string>("description"));
                }
            }
        }

        // -- parse commands
        if (object->has("commands")) {
            Poco::JSON::Object::Ptr commandsJson = object->getObject("commands");
            if (!commandsJson.isNull()) {

                // -- array of entry commands
                Poco::JSON::Array::Ptr commandsOnEntryJson = commandsJson->getArray("onStartSync");
                for (int i = 0; i < commandsOnEntryJson->size(); i++) {
                    Poco::Dynamic::Var commandParamsVar = commandsOnEntryJson->get(i);
                    Poco::JSON::Object::Ptr commandParams = commandParamsVar.extract<Poco::JSON::Object::Ptr>();
                    std::string typeStr = commandParams->getValue<std::string>("type");
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::generateFromString(typeStr)));

                    Poco::DynamicStruct dsParams = *commandParams;
                    std::set<std::string> setParams = dsParams.members();
                    std::set<std::string>::iterator itSetParams;
                    for (itSetParams = setParams.begin(); itSetParams != setParams.end(); itSetParams++) {
                        std::string key = std::string(itSetParams->c_str());
                        if (key != "type") {
                            std::string value = commandParams->getValue<std::string>(key);
                            cmd->addParameter(key, value);
                        }
                    }
                    commandsOnStartSync.push_back(cmd);
                }

                Poco::JSON::Array::Ptr commandsOnExitJson = commandsJson->getArray("onEndSync");
                for (int i = 0; i < commandsOnExitJson->size(); i++) {
                    Poco::Dynamic::Var commandParamsVar = commandsOnExitJson->get(i);
                    Poco::JSON::Object::Ptr commandParams = commandParamsVar.extract<Poco::JSON::Object::Ptr>();
                    std::string typeStr = commandParams->getValue<std::string>("type");
                    std::shared_ptr<Command> cmd = std::make_shared<Command>(Command(Command::generateFromString(typeStr)));

                    Poco::DynamicStruct dsParams = *commandParams;
                    std::set<std::string> setParams = dsParams.members();
                    std::set<std::string>::iterator itSetParams;
                    for (itSetParams = setParams.begin(); itSetParams != setParams.end(); itSetParams++) {
                        std::string key = std::string(itSetParams->c_str());
                        if (key != "type") {
                            std::string value = commandParams->getValue<std::string>(key);
                            cmd->addParameter(key, value);
                        }
                    }
                    commandsOnEndSync.push_back(cmd);
                }
            }
        }

    } catch (std::exception &e) {
        Poco::Logger::get("SyncFile").error("Exception while reading " + pathFile + " ! ");
        return false;
    }

    return true;
}
bool SyncFile::save()
{
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("version", version);
    root->set("type", type);
    root->set("itemSha1", sha1);
    root->set("cplUuid", cplId);
    root->set("startDelay", startDelay);
    root->set("entryPoint", entryPoint);
    root->set("cplDuration", cplDuration);
    root->set("cplEditRate", cplEditRate);
    root->set("title", cplTitle);
    root->set("comments", "");

    std::string projectionStr;
    if (typeProjection == PanelProjection::NO_FIT) {
        projectionStr = "NO_FIT";
    }
    else if (typeProjection == PanelProjection::FIT_X) {
        projectionStr = "FIT_X";
    }
    else if (typeProjection == PanelProjection::FIT_Y) {
        projectionStr = "FIT_Y";
    }
    else if (typeProjection == PanelProjection::FIT_X_PROP_Y) {
        projectionStr = "FIT_X_PROP_Y";
    }
    else if (typeProjection == PanelProjection::FIT_Y_PROP_X) {
        projectionStr = "FIT_Y_PROP_X";
    }
    else if (typeProjection == PanelProjection::FIT_X_FIT_Y) {
        projectionStr = "FIT_X_FIT_Y";
    }
    else {
        projectionStr = "UNKNOWN";
    }
    root->set("projection", projectionStr);

    Poco::JSON::Array::Ptr reelsJson = new Poco::JSON::Array();
    for (int i = 0; i < reels.size(); i++) {
        Poco::JSON::Object::Ptr reel = new Poco::JSON::Object();
        reel->set("id", reels.at(i).id);
        reel->set("duration", reels.at(i).duration);
        reel->set("editRate", reels.at(i).editRate);
        reel->set("keyId", reels.at(i).keyId);
        reel->set("hash", reels.at(i).hash);

        reelsJson->add(reel);
    }
    root->set("reels", reelsJson);

    if (cuts != nullptr) {
        Poco::JSON::Object::Ptr cutsJson = new Poco::JSON::Object();
        cutsJson->set("cplIdRef", cuts->getCplIdRef());
        cutsJson->set("cplTitleRef", cuts->getCplTitleRef());
        Poco::JSON::Array::Ptr scenesJson = new Poco::JSON::Array();
        for (int i = 0; i < cuts->getListOfCuts().size(); i++) {
            Poco::JSON::Object::Ptr sceneJson = new Poco::JSON::Object();
            sceneJson->set("startFrame", cuts->getListOfCuts().at(i).startFrame);
            sceneJson->set("endFrame", cuts->getListOfCuts().at(i).endFrame);
            sceneJson->set("description", cuts->getListOfCuts().at(i).description);
            scenesJson->add(sceneJson);
        }
        cutsJson->set("scenes", scenesJson);
        root->set("cuts", cutsJson);
    }

    // -- parse commands
    Poco::JSON::Object::Ptr cmdsJson = new Poco::JSON::Object();
    Poco::JSON::Array::Ptr cmdsOnStartJson = new Poco::JSON::Array();
    for (int i = 0; i < commandsOnStartSync.size(); i++) {
        Poco::JSON::Object::Ptr cmdStartJson = new Poco::JSON::Object();
        std::shared_ptr<Command> cmd(commandsOnStartSync.at(i));
        std::string typeStr = Command::generateToString(cmd->getType());
        cmdStartJson->set("type", typeStr);
        std::map<std::string, std::string>::iterator it;
        std::map<std::string, std::string> parameters(cmd->getParameters());
        for (it = parameters.begin(); it != parameters.end(); it++) {
            cmdStartJson->set(it->first, it->second);
        }
        cmdsOnStartJson->add(cmdStartJson);
    }
    cmdsJson->set("onStartSync", cmdsOnStartJson);

    Poco::JSON::Array::Ptr cmdsOnEndJson = new Poco::JSON::Array();
    for (int i = 0; i < commandsOnEndSync.size(); i++) {
        Poco::JSON::Object::Ptr cmdEndJson = new Poco::JSON::Object();
        std::shared_ptr<Command> cmd(commandsOnEndSync.at(i));
        std::string typeStr = Command::generateToString(cmd->getType());
        cmdEndJson->set("type", typeStr);
        std::map<std::string, std::string>::iterator it;
        std::map<std::string, std::string> parameters(cmd->getParameters());
        for (it = parameters.begin(); it != parameters.end(); it++) {
            cmdEndJson->set(it->first, it->second);
        }
        cmdsOnEndJson->add(cmdEndJson);
    }
    cmdsJson->set("onEndSync", cmdsOnEndJson);
    root->set("commands", cmdsJson);

    std::ostringstream oss;
    
    try {
        Poco::JSON::Stringifier::stringify(root, oss, 4);
    }
    catch (std::exception &e) {
        Poco::Logger::get("SyncFile").error("Exception while stringifying ! ");
        Poco::Logger::get("SyncFile").error(e.what());   
    }

    try {
        std::ofstream ofs(pathFile, std::ios_base::out | std::ios_base::trunc);
        if (ofs.is_open() == true)
        {
            ofs << oss.str();
            ofs.close();
        }
    }
    catch (std::exception &e) {
        Poco::Logger::get("SyncFile").error("Exception while saving " + pathFile + " ! ");
        return false;
    }

    return true;
}

int SyncFile::getFrameFromCplDurationMs(double currentMs)
{
    double timeMsInScript;

    // -- script start after CPL
    if (startDelay != 0) {
        double delayMs = (double)startDelay*1000.0 / speedRate;
        timeMsInScript = currentMs + delayMs;
    }

    // -- script start before CPL
    else if (entryPoint != 0) {
        double entryMs = (double)entryPoint*1000.0 / speedRate;
        timeMsInScript = currentMs - entryMs;
    }

    // -- take into account cuts
    if (cuts != nullptr) {
        for (int i = 0; i < cuts->getListOfCuts().size(); i++) {

            double startMsCutsInCpl = (double)cuts->getListOfCuts().at(i).startFrame*1000.0/speedRate;
            double endMsCutsInCpl = (double)cuts->getListOfCuts().at(i).endFrame*1000.0/speedRate;

            // -- if cuts started
            if (startMsCutsInCpl <= currentMs) {
                timeMsInScript += endMsCutsInCpl - startMsCutsInCpl + 1;
            }
        }
    }

    return timeMsInScript/1000.0 * speedRate;
}
