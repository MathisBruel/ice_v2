#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#pragma once

#include "ScriptInfoFile.h"

class ScriptState
{

public:

    ScriptState(int id_release) {this->id_release = id_release;}
    ~ScriptState() {}

    void addScriptFeature(int id_auditorium, std::shared_ptr<ScriptInfoFile> info) {scriptFeatureInfo.insert_or_assign(id_auditorium, info);}
    void addScriptLoop(int id_auditorium, std::shared_ptr<ScriptInfoFile> info) {scriptLoopInfo.insert_or_assign(id_auditorium, info);}
    void addScriptSas(int id_auditorium, std::shared_ptr<ScriptInfoFile> info) {scriptSasInfo.insert_or_assign(id_auditorium, info);}

    int getIdRelease() {return id_release;}

    std::map<int, std::shared_ptr<ScriptInfoFile>> getScriptFeatureInfo() {return scriptFeatureInfo;}
    std::map<int, std::shared_ptr<ScriptInfoFile>> getScriptLoopInfo() {return scriptLoopInfo;}
    std::map<int, std::shared_ptr<ScriptInfoFile>> getScriptSasInfo() {return scriptSasInfo;}

    std::string toXmlString();

private:

    int id_release;

    // -- for each server
    std::map<int, std::shared_ptr<ScriptInfoFile>> scriptFeatureInfo;
    std::map<int, std::shared_ptr<ScriptInfoFile>> scriptLoopInfo;
    std::map<int, std::shared_ptr<ScriptInfoFile>> scriptSasInfo;
};