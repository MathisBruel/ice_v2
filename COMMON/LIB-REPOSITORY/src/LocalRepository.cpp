#include "LocalRepository.h"

LocalRepository::LocalRepository(std::string pathBase)
{
    warning = false;
    this->pathBase = pathBase;
}
LocalRepository::~LocalRepository() {
    pathToScripts.clear();
    cplIdToScripts.clear();
}

bool LocalRepository::setPriorityScript(bool keep, std::string scriptName)
{
    std::map<std::string, std::shared_ptr<ScriptsInformations>>::iterator it;
    for (it = pathToScripts.begin(); it != pathToScripts.end(); it++) {

        if (scriptName == it->first) {

            Poco::Logger::get("LocalRepository").information("Change priority of script " + scriptName + " to " + std::to_string(keep), __FILE__, __LINE__);
            it->second->infos.setPriority(keep);

            Poco::Path pathInfoFile(pathBase + scriptName);
            pathInfoFile.setFileName("INFOS");
            
            if (!it->second->infos.save(pathInfoFile.toString())) {
                Poco::Logger::get("LocalRepository").error("Could not save INFOS file");
                return false;
            }
            return true;
        }
    }
    return false;
}

void LocalRepository::updateScriptsInformations()
{
    pathToScripts.clear();
    cplIdToScripts.clear();

    // -- fill list of all directory
    Poco::File directoryBase(pathBase);
    std::vector<std::string> listAll;
    std::vector<std::string> listDirectories;
    directoryBase.list(listAll);

    // -- keep only directories
    for (int i = 0; i < listAll.size(); i++) {

        Poco::Path pathDirectory(pathBase);
        pathDirectory.pushDirectory(listAll.at(i));

        Poco::File temp(pathDirectory);
        if (temp.isDirectory()) {
            listDirectories.push_back(pathDirectory.toString());
        }
    }

    // -- parse all directories
    for (int i = 0; i < listDirectories.size(); i++) {

        // -- fill list of files
        Poco::File directoryScript(listDirectories.at(i));
        std::vector<std::string> listFiles;
        directoryScript.list(listFiles);

        // -- create and start fill scipt informations
        ScriptsInformations script;
        std::shared_ptr<ScriptsInformations> newScript = std::make_shared<ScriptsInformations>(script);
        newScript->timestamp = directoryScript.getLastModified().epochMicroseconds();
        newScript->loop = false;
        newScript->playable = false;
        newScript->cisAvailable = false;
        newScript->percentCis = 0.0;
        newScript->lviAvailable = false;

        Poco::Logger::get("LocalRepository").information("Checking local directory : " + listDirectories.at(i), __FILE__, __LINE__);

        // -- tries to load info file
        // -- stop if not found
        if (!newScript->infos.load(listDirectories.at(i) + "INFOS")) {
            Poco::Logger::get("LocalRepository").error("Directory " + listDirectories.at(i) + " do not have infos file !", __FILE__, __LINE__);
            continue;
        }

        for (int j = 0; j < newScript->infos.getNbSync(); j++) {
            newScript->syncPresent.push_back(false);
        }
        
        // -- look all files in the directory
        for (int j = 0; j < listFiles.size(); j++) {

            Poco::Path pathFile(listDirectories.at(i));
            pathFile.setFileName(listFiles.at(j));
            Poco::File file(pathFile.toString());

            if (file.isFile()) {
                if (pathFile.getExtension().find("cis") != std::string::npos) { 

                    // -- check filename is ok
                    if (pathFile.getFileName() != newScript->infos.getSha1Cis() + ".cis") {
                        Poco::Logger::get("LocalRepository").warning("Cis file have different names : " + pathFile.getFileName() + " != " + newScript->infos.getSha1Cis() + ".cis", __FILE__, __LINE__);
                        file.remove(false);
                    }
                    
                    // -- check size
                    else if (file.getSize() != newScript->infos.getSizeCis()) {
                        Poco::Logger::get("LocalRepository").information("Cis file have different size : " + std::to_string(file.getSize()) + " != " + std::to_string(newScript->infos.getSizeCis()), __FILE__, __LINE__);
                        newScript->percentCis = (double)file.getSize() * 100.0 / (double)newScript->infos.getSizeCis();
                    }

                    else {
                        Poco::Logger::get("LocalRepository").information("Cis is available !", __FILE__, __LINE__);
                        newScript->percentCis = 100.0;
                        newScript->cisAvailable = true;
                    }
                }
                else if (pathFile.getExtension().find("lvi") != std::string::npos) {
                    
                    // -- check filename is ok
                    if (pathFile.getFileName() != newScript->infos.getSha1Cis() + ".lvi") {
                        Poco::Logger::get("LocalRepository").warning("Lvi file have different names : " + pathFile.getFileName() + " != " + newScript->infos.getSha1Cis() + ".lvi", __FILE__, __LINE__);
                        file.remove(false);
                    }
                    
                    // -- check size
                    else if (file.getSize() != newScript->infos.getSizeLvi()) {
                        Poco::Logger::get("LocalRepository").warning("Lvi file have different size : delete", __FILE__, __LINE__);
                        file.remove(false);
                    }

                    else {

                        Poco::Logger::get("LocalRepository").information("Lvi is available !", __FILE__, __LINE__);
                        newScript->lviAvailable = true;
                        std::ifstream ifs;
                        try {
                            ifs.open(pathFile.toString());

                            Poco::JSON::Parser parser;
                            Poco::Dynamic::Var result = parser.parse(ifs);
                            Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
                            std::string name = object->getValue<std::string>("loop");
                            std::string speed =  object->getValue<std::string>("fps");

                            if (name == "true") {
                                newScript->loop = true;
                            }
                            newScript->speedRate = std::stod(speed);
                            ifs.close();
                        
                        } catch (std::exception &e) {
                            Poco::Logger::get("LocalRepository").error("Error while parsing lvi file !", __FILE__, __LINE__);
                            if (ifs.is_open()) {
                                ifs.close();
                            }
                            newScript->lviAvailable = false;
                        }
                    }
                }
                else if (pathFile.getExtension().find("sync") != std::string::npos) {

                    // -- check filename is ok
                    bool found = false;
                    int idx = 0;
                    for (; idx < newScript->infos.getNbSync(); idx++) {
                        if (pathFile.getFileName() == newScript->infos.getSyncName(idx) + ".sync") {
                            found = true;
                            break;
                        }
                    }
                    
                    // -- existing in central repo
                    if (found) {
                        // -- check size
                        Poco::Logger::get("LocalRepository").information("Sync is central : " + pathFile.toString(), __FILE__, __LINE__);
                        if (file.getSize() != newScript->infos.getSyncSize(idx)) {
                            Poco::Logger::get("LocalRepository").warning("Sync file have different size : delete", __FILE__, __LINE__);
                            file.remove(false);
                        }
                        else {
                            cplIdToScripts.insert_or_assign(pathFile.getBaseName(), newScript);
                            newScript->syncPresent.at(idx) = true;
                        }
                    }
                    else {
                        //Poco::Logger::get("LocalRepository").debug("Sync is local : " + pathFile.toString(), __FILE__, __LINE__);
                        cplIdToScripts.insert_or_assign(pathFile.getBaseName(), newScript);
                    }
                }
            }
        }

        // -- script is considered as playable when 
        // -- the cis and lvi files are downloaded
        // -- we have the same size of datas between FTP and LOCAL REPO (can be higher if local synchro is done)
        if (newScript->cisAvailable && newScript->lviAvailable) {
            newScript->playable = true;
        }

        pathToScripts.insert_or_assign(newScript->infos.getNameScript(), newScript);
    }
}
void LocalRepository::checkMemoryUsage()
{
    Poco::File directoryBase(pathBase);
    diskSpaceTotal = directoryBase.totalSpace();
    diskSpaceFree = directoryBase.freeSpace();

    Poco::Logger::get("LocalRepository").information("Total space : " + std::to_string(diskSpaceTotal), __FILE__, __LINE__);
    Poco::Logger::get("LocalRepository").information("Free space : " + std::to_string(diskSpaceFree), __FILE__, __LINE__);

    // -- check if overused
    double percentUsed = (((double)diskSpaceTotal - (double)diskSpaceFree) / (double)diskSpaceTotal)*100.0;
    Poco::Logger::get("LocalRepository").information("Percent used : " + std::to_string(percentUsed), __FILE__, __LINE__);
    if (percentUsed > 80.0) {
        warning = true;
    }
}
void LocalRepository::allocateSpace()
{
    std::map<long, std::shared_ptr<ScriptsInformations>> suppressScripts;
    std::map<long, std::shared_ptr<ScriptsInformations>>::iterator itErase;
    std::map<std::string, std::shared_ptr<ScriptsInformations>>::iterator it;

    // -- fill map by timestamp to suppression for sorting
    for (it = pathToScripts.begin(); it != pathToScripts.end(); it++) {
        if (!it->second->infos.isToKeep()) {
            suppressScripts.insert_or_assign(it->second->timestamp, it->second);
        }
    }

    // -- parse by older timestamp to suppression
    for (itErase = suppressScripts.begin(); itErase != suppressScripts.end(); itErase++) {

        // -- delete elements
        Poco::File directoryToDelete(pathBase + "/" + it->second->infos.getNameScript());
        directoryToDelete.remove(true);

        // -- reclaculate disk usage
        checkMemoryUsage();
        // -- stop when disk usage is OK
        if (!warning) {
            break;
        }
    }

    updateScriptsInformations();
}

std::shared_ptr<LocalRepository::ScriptsInformations> LocalRepository::getScriptFromCplId(std::string cplId)
{
    std::map<std::string, std::shared_ptr<ScriptsInformations>>::iterator it = cplIdToScripts.find(cplId);
    
    if (it != cplIdToScripts.end()) {
        return it->second;
    }

    return nullptr;
}