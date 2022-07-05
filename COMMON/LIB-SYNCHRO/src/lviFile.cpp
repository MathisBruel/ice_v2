#include "lviFile.h"

LVI::LVI(std::string pathFile)
    : Synchronizable(pathFile)
{
    
}

LVI::~LVI()
{

}

bool LVI::load()
{
    Poco::File file(pathFile);
    if (!file.exists()) {
        Poco::Logger::get("LVI").error("File " + pathFile + " does not exists ! ", __FILE__, __LINE__);
        return false;
    }

    std::ifstream stream;
    Poco::Dynamic::Var result;

    try {
        stream.open(pathFile);
    
    } catch (std::exception &e) {
        Poco::Logger::get("LVI").error("Exception when opening Sync file !", __FILE__, __LINE__);
        return false;
    }

    try {
        Poco::JSON::Parser parser;
        result = parser.parse(stream);
    
    } catch (std::exception &e) {
        Poco::Logger::get("LVI").error("Exception when parsing Sync file !", __FILE__, __LINE__);
        Poco::Logger::get("LVI").error(e.what(), __FILE__, __LINE__);
        stream.close();
        return false;
    }

    stream.close();

    try {
        // -- we close now
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        // -- parse generic information
        version = object->getValue<std::string>("version");
        name = object->getValue<std::string>("name");
        type = object->getValue<std::string>("type");
        sha1 = object->getValue<std::string>("itemSha1");
        loop = object->getValue<bool>("loop");
        referenceFrame = object->getValue<std::string>("referenceFrame");
        fps = object->getValue<std::string>("fps");
        editRate = object->getValue<std::string>("editRate");
        duration = object->getValue<int>("duration");
        int posSep = editRate.find(" ", 0);
        if (posSep == std::string::npos) {
            Poco::Logger::get("LVI").error("Sync file has member cplEditRate badly formatted !");
            return false;
        }
        speedRate = std::stod(editRate.substr(0, posSep)) / std::stod(editRate.substr(posSep+1));

        // -- parse reels information
        Poco::JSON::Array::Ptr syncJson = object->getArray("sync");
        if (syncJson.isNull()) {
            Poco::Logger::get("LVI").error("Lvi file has no member sync !");
            return false;
        }

        // -- only parse first sync
        if (syncJson->size() <= 0) {
            Poco::Logger::get("LVI").error("Array of sync is empty !");
            return false;
        }
        Poco::Dynamic::Var resultSync = syncJson->get(0);
        Poco::JSON::Object::Ptr objectSync = resultSync.extract<Poco::JSON::Object::Ptr>();

        cplId = objectSync->getValue<std::string>("cplUuid");
        startDelay = objectSync->getValue<int>("startDelay");
        entryPoint = objectSync->getValue<int>("entryPoint");
        cplDuration = objectSync->getValue<int>("cplDuration");
        cplEditRate = objectSync->getValue<std::string>("cplEditRate");
        cplTitle = objectSync->getValue<std::string>("title");
        comments = objectSync->getValue<std::string>("comments");

        // -- parse reels information
        Poco::JSON::Array::Ptr reelsJson = objectSync->getArray("reels");
        if (reelsJson.isNull()) {
            Poco::Logger::get("LVI").error("Lvi file has no member reels !");
            return false;
        }

        for (int i = 0; i < reelsJson->size(); i++) {
            Poco::Dynamic::Var resultReel = reelsJson->get(i);
            Poco::JSON::Object::Ptr objectReel = resultReel.extract<Poco::JSON::Object::Ptr>();
            Synchronizable::Reel reel;
            reel.duration = objectReel->getValue<int>("duration");
            reel.editRate = objectReel->getValue<std::string>("editRate");
            int posSep = reel.editRate.find(" ", 0);
            if (posSep == std::string::npos) {
                Poco::Logger::get("LVI").error("Lvi file has member editRate badly formatted !");
                return false;
            }

            if (objectReel->has("id")) {
                reel.id = objectReel->getValue<std::string>("id");
            }
            if (objectReel->has("keyId")) {
                reel.keyId = objectReel->getValue<std::string>("keyId");
            }
            if (objectReel->has("hash")) {
                reel.hash = objectReel->getValue<std::string>("hash");
            }
            reel.speedRate = std::stod(reel.editRate.substr(0, posSep)) / std::stod(reel.editRate.substr(posSep+1));
            reels.push_back(reel);
        }
    } 
    catch (std::exception &e) {
        Poco::Logger::get("LVI").error("Exception while reading " + pathFile + " ! ");
        return false;
    }

    return true;
}
bool LVI::save()
{
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("version", version);
    root->set("name", name);
    root->set("type", type);
    root->set("itemSha1", sha1);
    root->set("loop", loop);
    root->set("referenceFrame", 0);
    root->set("fps", "24");
    root->set("editRate", "24 1");
    root->set("duration", duration);

    Poco::JSON::Array::Ptr syncJson = new Poco::JSON::Array();
    Poco::JSON::Object::Ptr sync = new Poco::JSON::Object();
    sync->set("cplUuid", cplId);
    sync->set("startDelay", startDelay);
    sync->set("entryPoint", entryPoint);
    sync->set("cplDuration", cplDuration);
    sync->set("cplEditRate", cplEditRate);
    sync->set("title", cplTitle);
    sync->set("comments", "");

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
    sync->set("reels", reelsJson);

    syncJson->add(sync);
    root->set("sync", syncJson);
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