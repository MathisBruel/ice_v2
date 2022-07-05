#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#pragma once

#include "ScriptInfoFile.h"

class DownloadedScripts
{

public:

    DownloadedScripts(int id_auditorium) {this->id_auditorium = id_auditorium;}
    ~DownloadedScripts() {}

    void addScript(std::shared_ptr<ScriptInfoFile> script) {scripts.push_back(script);}

    int getIdAuditorium() {return id_auditorium;}
    std::vector<std::shared_ptr<ScriptInfoFile>> getScripts() {return scripts;}

private:

    int id_auditorium;
    std::vector<std::shared_ptr<ScriptInfoFile>> scripts;
};