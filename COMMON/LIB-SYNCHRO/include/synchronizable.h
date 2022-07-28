#include <cstdlib>
#include <iostream>
#include <set>

#include "cutscenes.h"
#include "Poco/JSON/Parser.h"
#include "Poco/DynamicStruct.h"
#include <fstream>

#pragma once
#include "PanelProjection.h"
#include "command.h"

class Synchronizable
{

public:

    // -- generic object to handle synchronisation : LVI or CPL or SYNC

    // -- based only on picture asset
    struct Reel {
        std::string id;
        std::string editRate;
        double speedRate;
        int intrinsicDuration;
        int entryPoint;
        int duration;
        std::string keyId;
        std::string hash;
    };

    enum CommandEntry {
        ON_START_SYNC,
        ON_END_SYNC
    };

    Synchronizable(std::string pathFile);
    ~Synchronizable();
    
    virtual bool load() {return true;}
    virtual bool save() {return true;}

    // -- GENERAL PARAMETERS
    void setCplId(std::string cplId) {this->cplId = cplId;}
    std::string getCplId() {return cplId;}
    void setCplTitle(std::string cplTitle) {this->cplTitle = cplTitle;}
    std::string getCplTitle() {return cplTitle;}
    void setCplDuration(int cplDuration) {this->cplDuration = cplDuration;}
    int getCplDuration() {return cplDuration;}

    // -- RATE
    void setSpeedRate(double speedRate) {this->speedRate = speedRate;}
    double getSpeedRate() {return speedRate;}
    void setEditRate(std::string editRate) {this->cplEditRate = editRate;}

    // -- PROJECTION
    void setTypeProjection(PanelProjection::ProjectionType typeProjection) {this->typeProjection = typeProjection;}
    PanelProjection::ProjectionType getTypeProjection() {return typeProjection;}

    // -- CALCULATION DURATION
    double getTotalMsDuration(CutScenes* cuts);
    double getTotalMsDurationUntilIdx(int idx, CutScenes* cuts);
    double getTotalMsDurationFromIdx(int idx, CutScenes* cuts);
    double getTotalMsDurationBetweenIdx(int idxStart, int idxEnd, CutScenes* cuts);
    int convertToFrameFromIndexDecrement(double time, int endIdx);

    // -- REELS
    void addReel(Reel reel) {reels.push_back(reel);}
    std::vector<Reel> getReels() {return reels;}

    // -- CUTS
    void setCuts(CutScenes* cuts) {this->cuts = cuts;}
    CutScenes* getCuts() {return cuts;}

    // -- COMMANDS
    void addStartCommand(std::shared_ptr<Command> cmd);
    void addEndCommand(std::shared_ptr<Command> cmd);
    std::vector<std::shared_ptr<Command>> getCommandsOnStartSync() {return commandsOnStartSync;}
    std::vector<std::shared_ptr<Command>> getCommandsOnEndSync() {return commandsOnEndSync;}

    int getStartDelay() {return startDelay;}
    int getEntryPoint() {return entryPoint;}

protected:

    std::string pathFile;
    std::string cplId;
    std::string cplTitle;
    int cplDuration;
    std::string cplEditRate;

    int startDelay;
    int entryPoint;

    // comments
    double speedRate;
    PanelProjection::ProjectionType typeProjection;

    std::vector<Reel> reels;
    CutScenes* cuts;
    std::vector<std::shared_ptr<Command>> commandsOnStartSync;
    std::vector<std::shared_ptr<Command>> commandsOnEndSync;
};