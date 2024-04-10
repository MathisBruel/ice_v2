#include "Configuration.h"

Configuration::Configuration(std::string path)
{
    this->path = path;
    kinetConf = nullptr;
    imsConf = nullptr;
    iceConf = nullptr;
    lutConf = nullptr;
    repoConf = nullptr;
    sceneConf = nullptr;
}
Configuration::~Configuration() 
{
    if (kinetConf != nullptr) {
        delete kinetConf;
    }
    if (imsConf != nullptr) {
        delete imsConf;
    }
    if (iceConf != nullptr) {
        delete iceConf;
    }
    if (lutConf != nullptr) {
        delete lutConf;
    }
    if (repoConf != nullptr) {
        delete repoConf;
    }
    if (sceneConf != nullptr) {
        delete sceneConf;
    }
}

bool Configuration::loadConfiguration()
{
    if (kinetConf != nullptr) {
        delete kinetConf;
    }
    if (imsConf != nullptr) {
        delete imsConf;
    }
    if (iceConf != nullptr) {
        delete iceConf;
    }
    if (lutConf != nullptr) {
        delete lutConf;
    }
    if (repoConf != nullptr) {
        delete repoConf;
    }
    if (sceneConf != nullptr) {
        delete sceneConf;
    }

    Poco::Path pathDir(path);

    // -- check path of directory
    if (!pathDir.isDirectory()) {
        Poco::Logger::get("Configuration").error("Path of configuration files : " + path + " is not a directory ! ", __FILE__, __LINE__);
        return false;
    }

    // Poco::File directory(path);
    // if (!directory.exists()) {
    //     Poco::Logger::get("Configuration").error("Path of configuration files : " + path + " does not exists ! ", __FILE__, __LINE__);
    //     return false;
    // }

    // -- determine file path kinet conf and load
    Poco::Path kinetPath(path);
    kinetPath.setFileName("KinetConfiguration.xml");
    kinetConf = new KinetConfiguration(kinetPath.toString());
    Poco::Logger::get("Configuration").information("Loading KinetConfiguration.xml", __FILE__, __LINE__);
    if (!kinetConf->load()) {
        Poco::Logger::get("Configuration").error("Error when loading KinetConfiguration.xml !", __FILE__, __LINE__);
        return false;
    }

    // -- determine file path ims conf and load
    Poco::Path imsPath(path);
    imsPath.setFileName("IMSConfiguration.xml");
    imsConf = new IMSConfiguration(imsPath.toString());
    Poco::Logger::get("Configuration").information("Loading IMSConfiguration.xml", __FILE__, __LINE__);
    if (!imsConf->load()) {
        Poco::Logger::get("Configuration").error("Error when loading IMSConfiguration.xml !", __FILE__, __LINE__);
        return false;
    }

    // -- determine file path ice conf and load
    Poco::Path icePath(path);
    icePath.setFileName("IceConfiguration.xml");
    iceConf = new IceConfiguration(icePath.toString());
    Poco::Logger::get("Configuration").information("Loading IceConfiguration.xml", __FILE__, __LINE__);
    if (!iceConf->load()) {
        Poco::Logger::get("Configuration").error("Error when loading IceConfiguration.xml !", __FILE__, __LINE__);
        return false;
    }

    // -- determine file path lut conf and load
    Poco::Path lutPath(path);
    lutPath.setFileName("LutConfiguration.xml");
    lutConf = new LutConfiguration(lutPath.toString());
    Poco::Logger::get("Configuration").information("Loading LutConfiguration.xml", __FILE__, __LINE__);
    if (!lutConf->load()) {
        Poco::Logger::get("Configuration").error("Error when loading LutConfiguration.xml !", __FILE__, __LINE__);
        return false;
    }

    // -- determine file path lut conf and load
    Poco::Path repoPath(path);
    repoPath.setFileName("RepoConfiguration.xml");
    repoConf = new RepoConfiguration(repoPath.toString());
    Poco::Logger::get("Configuration").information("Loading RepoConfiguration.xml", __FILE__, __LINE__);
    if (!repoConf->load()) {
        Poco::Logger::get("Configuration").error("Error when loading RepoConfiguration.xml !", __FILE__, __LINE__);
        return false;
    }

    // -- determine file path scene conf and load
    Poco::Path scenePath(path);
    scenePath.setFileName("SceneConfiguration.xml");
    sceneConf = new SceneConfiguration(scenePath.toString());
    Poco::Logger::get("Configuration").information("Loading SceneConfiguration.xml", __FILE__, __LINE__);
    try {
        if (!sceneConf->load()) {
            Poco::Logger::get("Configuration").error("Error when loading SceneConfiguration.xml !", __FILE__, __LINE__);
            return false;
        }
    } catch (std::exception &e) {
        Poco::Logger::get("Configuration").error(e.what());
    }

    Poco::Logger::get("Configuration").information(this->toString(), __FILE__, __LINE__);

    return true;
}

std::string Configuration::toString()
{
    std::string conf = "KINET : \n";
    conf += kinetConf->toString() + "\n";

    conf += "IMS : \n";
    conf += imsConf->toString() + "\n";

    conf += "ICE : \n";
    conf += iceConf->toString() + "\n";

    conf += "LUT : \n";
    conf += lutConf->toString() + "\n";

    conf += "REPO : \n";
    conf += repoConf->toString() + "\n";

    return conf;
}