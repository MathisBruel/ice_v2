#include "ScriptInfoFile.h"

ScriptInfoFile::ScriptInfoFile()
{
    keep = false;
    alreadyPlayed = false;
    sizeCis = 0;
    sizeLvi = 0;
}

void ScriptInfoFile::addSync(std::string name, std::string sha1, int size)
{
    sync.push_back(name);
    sha1Sync.push_back(sha1);
    sizeSync.push_back(size);
}

bool ScriptInfoFile::changeDatas(ScriptInfoFile newScript)
{
    bool hasChanged = false;
    if (ftpPath != newScript.ftpPath) {
        ftpPath = newScript.ftpPath;
        hasChanged = true;
    }
    if (nameScript != newScript.nameScript) {
        nameScript = newScript.nameScript;
        hasChanged = true;
    }
    if (sha1Cis != newScript.sha1Cis) {
        sha1Cis = newScript.sha1Cis;
        hasChanged = true;
    }
    if (sizeCis != newScript.sizeCis) {
        sizeCis = newScript.sizeCis;
        hasChanged = true;
    }
    if (sha1Lvi != newScript.sha1Lvi) {
        sha1Lvi = newScript.sha1Lvi;
        hasChanged = true;
    }
    if (sizeLvi != newScript.sizeLvi) {
        sizeLvi = newScript.sizeLvi;
        hasChanged = true;
    }

    if (sync.size() != newScript.sync.size()) {
        sync.clear();
        sha1Sync.clear();
        sizeSync.clear();
        for (int i = 0; i < newScript.sync.size(); i++) {
            sync.push_back(newScript.sync.at(i));
            sha1Sync.push_back(newScript.sha1Sync.at(i));
            sizeSync.push_back(newScript.sizeSync.at(i));
        }
        hasChanged = true;
    }
    else {
        for (int i = 0; i < newScript.sync.size(); i++) {

            if (sync.at(i) != newScript.sync.at(i)) {
                sync.at(i) = newScript.sync.at(i);
                hasChanged = true;
            }
             if (sha1Sync.at(i) != newScript.sha1Sync.at(i)) {
                sha1Sync.at(i) = newScript.sha1Sync.at(i);
                hasChanged = true;
            }
             if (sizeSync.at(i) != newScript.sizeSync.at(i)) {
                sizeSync.at(i) = newScript.sizeSync.at(i);
                hasChanged = true;
            }
        }
    }

    return hasChanged;
}

bool ScriptInfoFile::load(std::string pathFile)
{
    Poco::File file(pathFile);

    if (!file.exists()) {
        Poco::Logger::get("ScriptInfoFile").error("File " + pathFile + " does not exists ! ", __FILE__, __LINE__);
        return false;
    }

    std::ifstream stream;
    Poco::Dynamic::Var result;

    try {
        stream.open(pathFile);
    
    } catch (std::exception &e) {
        Poco::Logger::get("ScriptInfoFile").error("Exception when openiong Sync file !", __FILE__, __LINE__);
        return false;
    }

    try {
        Poco::JSON::Parser parser;
        result = parser.parse(stream);
    
    } catch (std::exception &e) {
        Poco::Logger::get("ScriptInfoFile").error("Exception when parsing Sync file !", __FILE__, __LINE__);
        stream.close();
        return false;
    }

    stream.close();

    try {
        // -- we close now
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        // -- parse generic information
        keep = object->getValue<bool>("keep");
        alreadyPlayed = object->getValue<bool>("played");
        ftpPath = object->getValue<std::string>("ftpPath");
        nameScript = object->getValue<std::string>("nameScript");
        sha1Cis = object->getValue<std::string>("sha1Cis");
        sizeCis = object->getValue<long>("sizeCis");
        sha1Lvi = object->getValue<std::string>("sha1Lvi");
        sizeLvi = object->getValue<int>("sizeLvi");

        sync.clear();
        sha1Sync.clear();
        sizeSync.clear();

        Poco::JSON::Array::Ptr syncs = object->getArray("syncs");
        for (int i = 0; i < syncs->size(); i++) {
            Poco::Dynamic::Var syncVar = syncs->get(i);
            Poco::JSON::Object::Ptr syncObject = syncVar.extract<Poco::JSON::Object::Ptr>();
            sync.push_back(syncObject->getValue<std::string>("name"));
            sha1Sync.push_back(syncObject->getValue<std::string>("sha1"));
            sizeSync.push_back(syncObject->getValue<int>("size"));
        }

    } catch (std::exception &e) {
        Poco::Logger::get("ScriptInfoFile").error("Exception while reading " + pathFile + " ! ");
        return false;
    }

    return true;
}

bool ScriptInfoFile::save(std::string pathFile) {

    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("keep", keep);
    root->set("played", alreadyPlayed);
    root->set("ftpPath", ftpPath);
    root->set("nameScript", nameScript);
    root->set("sha1Cis", sha1Cis);
    root->set("sizeCis", sizeCis);
    root->set("sha1Lvi", sha1Lvi);
    root->set("sizeLvi", sizeLvi);

    Poco::JSON::Array::Ptr syncs = new Poco::JSON::Array();
    for (int i = 0; i < sync.size(); i++) {
        Poco::JSON::Object::Ptr syncNode = new Poco::JSON::Object();
        syncNode->set("name", sync.at(i));
        syncNode->set("sha1", sha1Sync.at(i));
        syncNode->set("size", sizeSync.at(i));
        syncs->add(syncNode);
    }
    root->set("syncs", syncs);

    std::ostringstream oss;
    Poco::JSON::Stringifier::stringify(root, oss);

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

std::string ScriptInfoFile::toXmlString()
{
    std::string xml = "<infoScript name=\"" + nameScript + "\">";
    xml += "<cis name=\"" + sha1Cis + ".cis\" percent=\"" + std::to_string(sizeCis) + "\" />";
    xml += "<lvi name=\"" + sha1Cis + ".lvi\" percent=\"" + std::to_string(sizeLvi) + "\" />";
    xml += "<syncs>";
    for (int i = 0; i < sync.size(); i++) {
        xml += "<sync name=\"" + sync.at(i) + "\" percent=\"" + std::to_string(sizeSync.at(i)) + "\" />";
    }
    xml += "</syncs>";
    xml += "</infoScript>";
    return xml;
}