#include <cstdlib>
#include <iostream>
#include <fstream>

#pragma once

#include "Poco/JSON/Parser.h"
#include "Poco/DynamicStruct.h"
#include "Poco/Logger.h"
#include "Poco/File.h"
#include "Poco/Path.h"

class ScriptInfoFile {

public:

    ScriptInfoFile();
    ~ScriptInfoFile() {}

    bool changeDatas(ScriptInfoFile newScript);

    bool load(std::string pathFile);
    bool save(std::string pathFile); 

    void setPriority(bool keep) {this->keep = keep;}
    void setAlreadyPlayed() {alreadyPlayed = true;}
    void setFtpPath(std::string ftpPath) {this->ftpPath = ftpPath;}
    void setNameScript(std::string nameScript) {this->nameScript = nameScript;} 
    void setCisInfo(std::string sha1Cis, long sizeCis) {this->sha1Cis = sha1Cis; this->sizeCis = sizeCis;}
    void setLviInfo(std::string sha1Lvi, int sizeLvi) {this->sha1Lvi = sha1Lvi; this->sizeLvi = sizeLvi;}
    void addSync(std::string name, std::string sha1, int size);

    bool isToKeep() {return keep;}
    bool isAlreadyPlayed() {return alreadyPlayed;}
    std::string getFtpPath() {return ftpPath;}
    std::string getNameScript() {return nameScript;}
    std::string getSha1Cis() {return sha1Cis;}
    long getSizeCis() {return sizeCis;}
    std::string getSha1Lvi() {return sha1Lvi;}
    int getSizeLvi() {return sizeLvi;}
    int getNbSync() {return sync.size();}
    std::string getSyncName(int idx) {return sync.at(idx);}
    std::string getSyncSha1(int idx) {return sha1Sync.at(idx);}
    int getSyncSize(int idx) {return sizeSync.at(idx);}

    std::string toXmlString();

private:

    bool keep;
    bool alreadyPlayed;

    std::string ftpPath;

    std::string nameScript;
    std::string sha1Cis;
    long sizeCis;
    std::string sha1Lvi;
    int sizeLvi;

    std::vector<std::string> sync;
    std::vector<std::string> sha1Sync;
    std::vector<int> sizeSync;
};