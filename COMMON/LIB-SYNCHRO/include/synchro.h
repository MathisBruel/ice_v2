#include <cstdlib>
#include <iostream>
#include <vector>
#include "cplFile.h"
#include "syncFile.h"
#include "synchronizable.h"
#include "cutscenes.h"
#include "Poco/Logger.h"

#pragma once

class Synchro
{

public:

    // -- object that actually synchronize

    struct Match {
        int idxReelRef;
        int nbIdxReelRef;
        int idxReelNew;
        int nbIdxReelNew; // -- or index for final match list
        double duration; // -- in milliseconds
    };

    struct Match2 {
        double startMsMatchRef;
        double endMsMatchRef;
        double startMsMatchNew;
        double endMsMatchNew;
        int offsetFrameInReel;
        int durationFrame;
        int idxReelRef;
        int idxReelNew;
    };

    struct InterMatch {
        double startMsMatchRef;
        double endMsMatchRef;
        double startMsMatchNew;
        double endMsMatchNew;
        double startMsMatchRefUncut;
        double endMsMatchRefUncut;
        bool valid;
    };

    struct InvalidCut {
        double startMs;
        double endMs;
        bool left;
        bool right;
        int nbFrameOutLeft;
        int nbFrameOutRight;
        int nbMissingFrame;
    };

    Synchro(Synchronizable* synchronizable, CplFile* newCpl, CutScenes* cuts);
    ~Synchro();

    bool isSynchronized() {return synchronized;}
    void synchronizeByDuration();
    void synchronizeByPictureId();
    std::string matchesToString();

    Synchronizable* getSynchronizable() {return synchronizable;}
    CplFile* getNewCpl() {return newCpl;}
    CutScenes* getCuts() {return cuts;}

    SyncFile* generateSyncFile(std::string pathFile);
    std::string getOutputs() {return outputs;}

    int getEntryPoint() {return entryPoint;}
    int getStartDelay() {return startDelay;}

    std::vector<Match> getMatch() {return matches;}
    std::vector<Match2> getMatch2() {return matches2;}
    std::vector<InterMatch> getInter() {return interMatches;}
    std::vector<InvalidCut> getInvalidCuts() {return invalidCuts;}

    void tryFindSamePictureId();
    void tryMatchMulti();
    void fillMatches();
    void checkFilled();
    void checkFilled2();

    std::string toXmlString(bool byId);

private:

    const double epsilon = 1e-6;

    Synchronizable* synchronizable;
    CplFile* newCpl;
    CutScenes* cuts;

    bool synchronized;
    int entryPoint;
    int startDelay;

    std::string outputs;
    std::vector<Match> matches;
    std::vector<Match2> matches2;
    std::vector<InterMatch> interMatches;
    std::vector<InvalidCut> invalidCuts;
};