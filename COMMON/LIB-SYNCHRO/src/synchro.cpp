#include "synchro.h"

Synchro::Synchro(Synchronizable* synchronizable, CplFile* newCpl, CutScenes* cuts)
{
    this->synchronizable = synchronizable;
    this->newCpl = newCpl;
    this->cuts = cuts;
}
Synchro::~Synchro() {}

void Synchro::synchronizeByDuration()
{
    startDelay = -1;
    entryPoint = -1;
    synchronized = true;
    outputs = "";

    double totalDurationRef = synchronizable->getTotalMsDuration(cuts);
    double totalDurationNew = newCpl->getTotalMsDuration(nullptr);

    if (std::abs(totalDurationNew - totalDurationRef) > 60000) {
        Poco::Logger::get("Synchro").error("Total Duration are too different. New : " + std::to_string(totalDurationNew) + ", Ref : " + std::to_string(totalDurationRef), __FILE__, __LINE__);
        outputs += "Total Duration are too different. New : " + std::to_string(totalDurationNew) + ", Ref : " + std::to_string(totalDurationRef) + "\n";
        synchronized = false;
        return;
    }

    tryMatchMulti();
    //Poco::Logger::get("Synchro").debug(matchesToString(), __FILE__, __LINE__);
    outputs += matchesToString() + "\n";

    if (matches.size() == 0) {
        Poco::Logger::get("Synchro").error("No match of reel at all : impossible to sync !", __FILE__, __LINE__);
        outputs += "No match of reel at all : impossible to sync !\n";
        synchronized = false;
        return;
    }

    fillMatches();
    //Poco::Logger::get("Synchro").debug(matchesToString(), __FILE__, __LINE__);
    outputs += matchesToString() + "\n";

    checkFilled();
    //Poco::Logger::get("Synchro").debug("Start delay : " + std::to_string(startDelay), __FILE__, __LINE__);
    //Poco::Logger::get("Synchro").debug("Entry point : " + std::to_string(entryPoint), __FILE__, __LINE__);
    outputs += "Start delay : " + std::to_string(startDelay) + "\n";
    outputs += "Entry point : " + std::to_string(entryPoint) + "\n";
}

void Synchro::synchronizeByPictureId()
{
    startDelay = -1;
    entryPoint = -1;
    synchronized = true;
    outputs = "";

    double totalDurationRef = synchronizable->getTotalMsDuration(cuts);
    double totalDurationNew = newCpl->getTotalMsDuration(nullptr);

    if (std::abs(totalDurationNew - totalDurationRef) > 60000) {
        Poco::Logger::get("Synchro").error("Total Duration are too different. New : " + std::to_string(totalDurationNew) + ", Ref : " + std::to_string(totalDurationRef), __FILE__, __LINE__);
        outputs += "Total Duration are too different. New : " + std::to_string(totalDurationNew) + ", Ref : " + std::to_string(totalDurationRef) + "\n";
        synchronized = false;
        return;
    }

    tryFindSamePictureId();

    // -- check at least one match exists
    if (matches2.size() <= 0) {
        Poco::Logger::get("Synchro").error("No match exists on pictureAsset id : synchronization impossible !");
        outputs += "No match exists on pictureAsset id : synchronization impossible !\n";
        synchronized = false;
        return;
    }

    checkFilled2();
}

void Synchro::tryFindSamePictureId()
{
    // -- reinit structures
    synchronized = true;
    matches2.clear();
    interMatches.clear();
    invalidCuts.clear();
    startDelay = 0;
    entryPoint = 0;

    double speedRateRefFinal = synchronizable->getSpeedRate();

    for (int i = 0; i < synchronizable->getReels().size(); i++) {
        for (int j = 0; j < newCpl->getReels().size(); j++) {
            if (synchronizable->getReels().at(i).id == newCpl->getReels().at(j).id) {

                if (synchronizable->getSpeedRate() == -1.0) {
                    speedRateRefFinal = synchronizable->getReels().at(i).speedRate;
                }

                Match2 match;
                match.offsetFrameInReel = std::max(synchronizable->getReels().at(i).entryPoint, newCpl->getReels().at(j).entryPoint);
                match.durationFrame = std::min(synchronizable->getReels().at(i).duration + synchronizable->getReels().at(i).entryPoint, 
                                            newCpl->getReels().at(j).duration + newCpl->getReels().at(j).entryPoint);
                match.idxReelRef = i;
                match.idxReelNew = j;

                match.startMsMatchRef = synchronizable->getTotalMsDurationUntilIdx(i, nullptr) + 
                    (double)(match.offsetFrameInReel - synchronizable->getReels().at(i).entryPoint)*1000.0 / speedRateRefFinal;
                match.startMsMatchNew = newCpl->getTotalMsDurationUntilIdx(j, nullptr) + 
                    (double)(match.offsetFrameInReel - newCpl->getReels().at(j).entryPoint)* 1000.0 / newCpl->getReels().at(j).speedRate;
                
                match.endMsMatchRef = match.startMsMatchRef + ((double)(match.durationFrame - match.offsetFrameInReel - 1)*1000.0 / speedRateRefFinal);
                match.endMsMatchNew = match.startMsMatchNew + ((double)(match.durationFrame - match.offsetFrameInReel - 1)*1000.0 / newCpl->getReels().at(j).speedRate);

                matches2.push_back(match);

                outputs += "Match found on picture asset id " + synchronizable->getReels().at(i).id + "\n";
                outputs += "Start frame match in reel : " + std::to_string(match.offsetFrameInReel) + "\n";
                outputs += "Minimum duration match between reels : " + std::to_string(match.durationFrame-match.offsetFrameInReel) + "\n";
            }
        }
    }
}

void Synchro::checkFilled2()
{

    for (int i = 0 ; i < matches2.size(); i++) {
        if (i == 0) {
            
            double startMatchMsRef = matches2.at(i).startMsMatchRef;
            double startMatchMsNew = matches2.at(i).startMsMatchNew;
            double cutsMs = 0.0;

            if (cuts != nullptr) {
                double speedRate = synchronizable->getSpeedRate();
                for (int j = 0; j < cuts->getListOfCuts().size(); j++) {
                    
                    if (speedRate == -1.0) {
                        speedRate = synchronizable->getReels().at(matches2.at(i).idxReelRef).speedRate;
                    }
                    CutScenes::Cut& cut = cuts->getListOfCuts().at(j);

                    double startMsCut = (double)(cut.startFrame)*1000.0/speedRate;
                    double endMsCut = (double)(cut.endFrame)*1000.0/speedRate;

                    // -- cut inside reel
                    if (endMsCut < startMatchMsRef) {
                        cutsMs = (endMsCut - startMsCut) + 1000.0/speedRate;
                        //Poco::Logger::get("Synchro").debug("Cuts on first match reel : " + std::to_string((endMsCut - startMsCut) + 1000.0/speedRate), __FILE__, __LINE__);
                    }

                    // -- cut part at begin of reel
                    else if (startMsCut < startMatchMsRef && endMsCut >= startMatchMsRef) {
                        InvalidCut invalid;
                        invalid.startMs = startMsCut;
                        invalid.endMs = endMsCut;
                        invalid.left = false;
                        invalid.right = true;
                        invalid.nbFrameOutRight = (double)(endMsCut - startMatchMsRef)*speedRate/1000.0 + 1.5;
                        invalidCuts.push_back(invalid);
                        synchronized = false;
                        Poco::Logger::get("Synchro").error("Invalid cut : " + std::to_string(invalid.nbFrameOutLeft), __FILE__, __LINE__);
                        continue;
                    }
                }
            }

            if (std::abs(startMatchMsRef - cutsMs - startMatchMsNew) > 60000.0) {
                //Poco::Logger::get("Synchro").debug("Match not valid : time between matches at begin not coherent !", __FILE__, __LINE__);
                outputs += "Match not valid : time between matches at begin not coherent : " + 
                std::to_string(startMatchMsNew) + "/" + std::to_string(startMatchMsRef) + "\n";
                synchronized = false;

                InterMatch inter;
                inter.startMsMatchRef = 0;
                inter.startMsMatchNew = 0;
                inter.endMsMatchRef = startMatchMsRef;
                inter.endMsMatchNew = startMatchMsNew;
                inter.valid = false;
                interMatches.push_back(inter);
            }
            else {

                if (startMatchMsRef != 0 && startMatchMsNew != 0) {
                    InterMatch inter;
                    inter.startMsMatchRef = 0;
                    inter.startMsMatchNew = 0;
                    inter.endMsMatchRef = startMatchMsRef;
                    inter.endMsMatchNew = startMatchMsNew;
                    inter.valid = true;
                    interMatches.push_back(inter);
                }

                if (startMatchMsNew == startMatchMsRef) {
                    //Poco::Logger::get("Synchro").debug("Duration at start matches : no entry point or start Delay !", __FILE__, __LINE__);
                    entryPoint = 0;
                    startDelay  = 0;
                }
                else if (startMatchMsNew > startMatchMsRef) {
                    entryPoint = (startMatchMsNew - startMatchMsRef)/1000.0 * newCpl->getReels().at(i).speedRate + 0.5;
                    //Poco::Logger::get("Synchro").debug("EntryPoint set : " + std::to_string(entryPoint), __FILE__, __LINE__);
                }
                else {
                    startDelay = (startMatchMsRef - startMatchMsNew)/1000.0 * newCpl->getReels().at(i).speedRate + 0.5;
                    //Poco::Logger::get("Synchro").debug("StartDelay set : " + std::to_string(startDelay), __FILE__, __LINE__);
                }
            }

        }
        else {
            double endLastMatchMsRef = matches2.at(i-1).endMsMatchRef;
            double endLastMatchMsNew = matches2.at(i-1).endMsMatchNew;
            double startMatchMsRef = matches2.at(i).startMsMatchRef;
            double startMatchMsNew = matches2.at(i).startMsMatchNew;

            double diffRef = startMatchMsRef - endLastMatchMsRef;
            double diffNew = startMatchMsNew - endLastMatchMsNew;
            double cutsMs = 0.0;
            double speedRate = synchronizable->getSpeedRate();
            if (speedRate == -1.0) {
                speedRate = synchronizable->getReels().at(matches2.at(i).idxReelRef).speedRate;
            }

            CutScenes::Cut* cutRetained = nullptr;
            if (cuts != nullptr) {
                for (int j = 0; j < cuts->getListOfCuts().size(); j++) {
                    
                    CutScenes::Cut& cut = cuts->getListOfCuts().at(j);

                    double startMsCut = (double)(cut.startFrame)*1000.0/speedRate;
                    double endMsCut = (double)(cut.endFrame)*1000.0/speedRate;

                    //Poco::Logger::get("Synchro").debug("startMsCut : " + std::to_string(startMsCut), __FILE__, __LINE__);
                    //Poco::Logger::get("Synchro").debug("endMsCut : " + std::to_string(endMsCut), __FILE__, __LINE__);
                    //Poco::Logger::get("Synchro").debug("endLastMatchMsRef : " + std::to_string(endLastMatchMsRef), __FILE__, __LINE__);
                    //Poco::Logger::get("Synchro").debug("startMatchMsRef : " + std::to_string(startMatchMsRef), __FILE__, __LINE__);

                    // -- cut inside reel
                    if (startMsCut > endLastMatchMsRef && endMsCut < startMatchMsRef) {
                        cutsMs = (endMsCut - startMsCut) + 1000.0/speedRate;
                        cutRetained = &cut;
                        //Poco::Logger::get("Synchro").debug("Cuts on matched reel 1 : " + std::to_string(cutsMs), __FILE__, __LINE__);
                    }

                    // -- cut part at end of reel
                    else if (startMsCut > endLastMatchMsRef && endMsCut >= startMatchMsRef && startMsCut < startMatchMsRef) {
                        Poco::Logger::get("Synchro").error("Invalid cut !", __FILE__, __LINE__);
                        InvalidCut invalid;
                        invalid.startMs = startMsCut;
                        invalid.endMs = endMsCut;
                        invalid.left = false;
                        invalid.right = true;
                        invalid.nbFrameOutRight = (double)(endMsCut - startMatchMsRef)*speedRate/1000.0 + 1.5;
                        invalidCuts.push_back(invalid);
                        synchronized = false;
                        continue;
                    }

                    else if (startMsCut <= endLastMatchMsRef && endMsCut < startMatchMsRef && endMsCut > endLastMatchMsRef) {
                        Poco::Logger::get("Synchro").error("Invalid cut !", __FILE__, __LINE__);
                        InvalidCut invalid;
                        invalid.startMs = startMsCut;
                        invalid.endMs = endMsCut;
                        invalid.left = true;
                        invalid.right = false;
                        invalid.nbFrameOutLeft = (double)(endLastMatchMsRef - startMsCut)*speedRate/1000.0 + 1.5;
                        invalidCuts.push_back(invalid);
                        synchronized = false;
                        continue;
                    }

                    else if (startMsCut <= endLastMatchMsRef && endMsCut >= startMatchMsRef) {
                        Poco::Logger::get("Synchro").error("Invalid cut !", __FILE__, __LINE__);
                        InvalidCut invalid;
                        invalid.startMs = startMsCut;
                        invalid.endMs = endMsCut;
                        invalid.left = true;
                        invalid.right = false;
                        invalid.nbFrameOutLeft = (double)(endLastMatchMsRef - startMsCut + 2)*speedRate/1000.0 + 1.5;
                        invalidCuts.push_back(invalid);
                        synchronized = false;
                        continue;
                    }
                }
            }

            if (std::abs(diffRef - cutsMs - diffNew) > epsilon) {
                //Poco::Logger::get("Synchro").debug("Match not valid : time between matches in not coherent : " + 
                //std::to_string(diffNew) + "/" + std::to_string(diffRef - cutsMs), __FILE__, __LINE__);
                outputs += "Match not valid : time between matches in not coherent : " + 
                std::to_string(diffNew) + "/" + std::to_string(diffRef - cutsMs) + "\n";
                synchronized = false;

                if (cutsMs != 0) {
                    InvalidCut invalid;
                    invalid.startMs = (double)(cutRetained->startFrame)*1000.0/speedRate;
                    invalid.endMs = (double)(cutRetained->endFrame)*1000.0/speedRate;
                    invalid.left = true;
                    invalid.right = true;
                    invalid.nbMissingFrame = (double)(std::abs(diffRef - cutsMs - diffNew))*speedRate/1000.0 + 0.5;
                    invalidCuts.push_back(invalid);
                }
                else {
                    InterMatch inter;
                    inter.startMsMatchRef = endLastMatchMsRef;
                    inter.startMsMatchNew = endLastMatchMsNew;
                    inter.endMsMatchRef = startMatchMsRef;
                    inter.endMsMatchNew = startMatchMsNew;
                    inter.valid = false;
                    interMatches.push_back(inter);
                }
            }
            else {

                if (diffRef > 1000.0/speedRate + epsilon) {
                    InterMatch inter;
                    inter.startMsMatchRef = endLastMatchMsRef;
                    inter.startMsMatchNew = endLastMatchMsNew;
                    inter.endMsMatchRef = startMatchMsRef;
                    inter.endMsMatchNew = startMatchMsNew;
                    inter.valid = true;
                    interMatches.push_back(inter);
                }
            }
        }
    
        if (i == matches2.size() - 1) {

            double totalDurationMsRef = synchronizable->getTotalMsDuration(nullptr);
            double totalDurationMsNew = newCpl->getTotalMsDuration(nullptr);
            double endMatchMsRef = matches2.at(i).endMsMatchRef;
            double endMatchMsNew = matches2.at(i).endMsMatchNew;

            double diffRef = totalDurationMsRef - endMatchMsRef;
            double diffNew = totalDurationMsNew - endMatchMsNew;
            double cutsMs = 0.0;

            std::cout << "match " << i << " : " << totalDurationMsRef << " - " << endMatchMsRef << std::endl;
            std::cout << "match " << i << " : " << totalDurationMsNew << " - " << endMatchMsNew << std::endl;
            double speedRate = synchronizable->getSpeedRate();
            if (speedRate == -1.0) {
                speedRate = synchronizable->getReels().at(matches2.at(i).idxReelRef).speedRate;
            }

            if (cuts != nullptr) {
                for (int j = 0; j < cuts->getListOfCuts().size(); j++) {
                    
                    CutScenes::Cut& cut = cuts->getListOfCuts().at(j);

                    double startMsCut = (double)(cut.startFrame)*1000.0/speedRate;
                    double endMsCut = (double)(cut.endFrame)*1000.0/speedRate;

                    // -- cut inside reel
                    if (startMsCut > endMatchMsRef) {
                        cutsMs = (endMsCut - startMsCut) + 1000.0/speedRate;
                        //Poco::Logger::get("Synchro").debug("Cuts on last matched reel !", __FILE__, __LINE__);
                    }

                    // -- cut part at begin of reel
                    // -- cut part at end of reel
                    else if (startMsCut <= endMatchMsRef && endMsCut > endMatchMsRef) {
                        Poco::Logger::get("Synchro").error("Invalid cut !", __FILE__, __LINE__);
                        InvalidCut invalid;
                        invalid.startMs = startMsCut;
                        invalid.endMs = endMsCut;
                        invalid.left = true;
                        invalid.right = false;
                        invalid.nbFrameOutLeft = (double)(endMatchMsRef - startMsCut)*speedRate/1000.0 + 1.5;
                        invalidCuts.push_back(invalid);
                        synchronized = false;
                        continue;
                    }
                }
            }

            if (std::abs(diffRef - cutsMs - diffNew) > 60000.0) {
                //Poco::Logger::get("Synchro").debug("Match not valid : time between matches at the end in too important !", __FILE__, __LINE__);
                outputs += "Match not valid : time between matches at the end in too important : " + 
                std::to_string(diffNew) + "/" + std::to_string(diffRef) + "\n";
                synchronized = false;

                InterMatch inter;
                inter.startMsMatchRef = endMatchMsRef;
                inter.startMsMatchNew = endMatchMsNew;
                inter.endMsMatchRef = totalDurationMsRef;
                inter.endMsMatchNew = totalDurationMsNew;
                inter.valid = false;
                interMatches.push_back(inter);
            }

            if (std::abs(diffRef - cutsMs - diffNew) > epsilon) {
                //Poco::Logger::get("Synchro").debug("Match varies : check if no cuts happen on last Reel !" + 
                //std::to_string(diffNew) + "/" + std::to_string(diffRef) + "\n", __FILE__, __LINE__);

                InterMatch inter;
                inter.startMsMatchRef = endMatchMsRef;
                inter.startMsMatchNew = endMatchMsNew;
                inter.endMsMatchRef = totalDurationMsRef;
                inter.endMsMatchNew = totalDurationMsNew;
                inter.valid = true;
                interMatches.push_back(inter);
            }
            else {
                if (diffNew > 1000.0/speedRate + epsilon) {
                    InterMatch inter;
                    inter.startMsMatchRef = endMatchMsRef;
                    inter.startMsMatchNew = endMatchMsNew;
                    inter.endMsMatchRef = totalDurationMsRef;
                    inter.endMsMatchNew = totalDurationMsNew;
                    inter.valid = true;
                    interMatches.push_back(inter);
                }
            }
        }
    }
}

void Synchro::tryMatchMulti()
{
    matches.clear();
    interMatches.clear();
    synchronized = true;
    startDelay = 0;
    entryPoint = 0;

    // -- try corresponding 1:n from ref CPL into 1:n in new CPL
    int idxRef = 0;
    while (idxRef < synchronizable->getReels().size()) {
        for (int i = 0; i < synchronizable->getReels().size()-idxRef; i++) {

            // -- build by increment all possible matches
            double durationRefReel = synchronizable->getTotalMsDurationBetweenIdx(idxRef, idxRef+i, cuts);

            int idxNew = 0;
            while (idxNew < newCpl->getReels().size()) {

                // -- try corresponding 1:n from ref CPL into 1:n in new CPL
                for (int j = 0; j < newCpl->getReels().size()-idxNew; j++) {
                    double durationNewReel = newCpl->getTotalMsDurationBetweenIdx(idxNew, idxNew+j, nullptr);

                    std::cout << "Duration NEW " << idxNew << " --> " << (idxNew+j) << " : " << durationNewReel << std::endl;

                    // -- correspondance found
                    if (std::abs(durationRefReel - durationNewReel) < epsilon) {

                        // -- fill match
                        Match match;
                        match.idxReelRef = idxRef;
                        match.idxReelNew = idxNew;
                        match.nbIdxReelRef = i+1;
                        match.nbIdxReelNew = j+1;

                        // -- we stop the loop in new only
                        idxNew = newCpl->getReels().size();
                        //Poco::Logger::get("Synchro").debug("Match found : reel ref [" + std::to_string(match.idxReelRef) + ";" + std::to_string(match.idxReelRef+match.nbIdxReelRef-1) +
                        //"] matches reel new [" + std::to_string(match.idxReelNew) + ";" + std::to_string(match.idxReelNew+match.nbIdxReelNew-1) + "]\n", __FILE__, __LINE__);
                        outputs += "Match found : reel ref [" + std::to_string(match.idxReelRef) + ";" + std::to_string(match.idxReelRef+match.nbIdxReelRef-1) +
                        "] matches reel new [" + std::to_string(match.idxReelNew) + ";" + std::to_string(match.idxReelNew+match.nbIdxReelNew-1) + "]\n";
                        matches.push_back(match);
                        break;
                    }

                    else if (durationNewReel > durationRefReel) {
                        break;
                    }
                }
                idxNew++;
            }
        }
        idxRef++;
    }

    // -- we check coherence before and after in duration with match as reference
    /*std::vector<Match>::iterator it;
    for (it = matches.begin(); it != matches.end(); it++) {

        // -- check correspondance of time before match (duration of match included)
        double durationRefBefore = synchronizable->getTotalMsDurationUntilIdx(it->idxReelRef, cuts);
        double durationNewBefore = newCpl->getTotalMsDurationUntilIdx(it->idxReelNew, nullptr);
        if (durationRefBefore != -1 && durationNewBefore != -1) {
            if (std::abs(durationRefBefore - durationNewBefore) > 60000.0) {
                it = matches.erase(it);
                Poco::Logger::get("Synchro").debug("ERASE BEFORE !", __FILE__, __LINE__);
                outputs += "Match not valid (duration before match is not coherent): erasing ! Duration before match ref/new : " + 
                std::to_string(durationRefBefore) + "/" + std::to_string(durationNewBefore) + "\n";
                continue;
            }
        }

        // -- check correspondance of time before match (duration of match included)
        double durationRefAfter = synchronizable->getTotalMsDurationFromIdx(it->idxReelRef+it->nbIdxReelRef-1, cuts);
        double durationNewAfter = newCpl->getTotalMsDurationFromIdx(it->idxReelNew+it->nbIdxReelNew-1, nullptr);

        if (durationRefAfter != -1 && durationNewAfter != -1) {
            if (std::abs(durationRefAfter - durationNewAfter) > 60000.0) {

                std::cout << "Erase " << std::endl;
                std::cout << it->idxReelRef << " --> " << (it->idxReelRef+it->nbIdxReelRef-1) << std::endl;
                std::cout << it->idxReelNew << " --> " << (it->idxReelNew+it->nbIdxReelNew-1) << std::endl;
                std::cout << durationRefAfter << " / " << durationNewAfter << std::endl;

                it = matches.erase(it);
                Poco::Logger::get("Synchro").debug("ERASE AFTER !", __FILE__, __LINE__);
                outputs += "Match not valid (duration after match is not coherent): erasing ! Duration after match ref/new : " + 
                std::to_string(durationRefAfter) + "/" + std::to_string(durationNewAfter) + "\n";
                continue;
            }
        }
    }*/
}

void Synchro::fillMatches()
{
    // -- first fill 1:n matches
    std::vector<Match>::iterator it;

    // -- initialize final structure
    std::vector<Match> finalMatches;
    for (int i = 0; i < synchronizable->getReels().size(); i++) {
        Match temp;
        temp.idxReelRef = i;
        temp.idxReelNew = -1;
        temp.nbIdxReelRef = 1;
        temp.nbIdxReelNew = -1;
        finalMatches.push_back(temp);
    }

    // -- parse all matches
    for (it = matches.begin(); it != matches.end(); it++) {

        int idxStartRef = it->idxReelRef;
        int idxEndRef = it->idxReelRef+it->nbIdxReelRef-1;

        //Poco::Logger::get("Synchro").debug("Treat match " + std::to_string(idxStartRef) + " - " + std::to_string(idxEndRef) + " : " + std::to_string(it->idxReelNew) + " - " + std::to_string(it->idxReelNew+it->nbIdxReelNew-1), __FILE__, __LINE__);

        // -- fill start in start
        if (finalMatches.at(idxStartRef).idxReelNew == -1) {
            finalMatches.at(idxStartRef).idxReelNew = it->idxReelNew;
        }
        else if (finalMatches.at(idxStartRef).idxReelNew != it->idxReelNew) {
            //Poco::Logger::get("Synchro").debug("Error mismatched in start of start. Current : " + std::to_string(finalMatches.at(idxStartRef).idxReelNew) + ", new : " + std::to_string(it->idxReelNew), __FILE__, __LINE__);
            outputs += "Error mismatched in start of start. Current : " + std::to_string(finalMatches.at(idxStartRef).idxReelNew) + ", new : " + std::to_string(it->idxReelNew) + "\n";
            synchronized = false;
            return;
        }

        // -- fill start of end
        if (finalMatches.at(idxEndRef).nbIdxReelNew == -1) {
            finalMatches.at(idxEndRef).nbIdxReelNew = it->idxReelNew+it->nbIdxReelNew-1;
        }
        else if (finalMatches.at(idxEndRef).nbIdxReelNew != it->idxReelNew+it->nbIdxReelNew-1) {
            //Poco::Logger::get("Synchro").debug("Error mismatched in start of end. Current : " + std::to_string(finalMatches.at(idxEndRef).nbIdxReelNew) + ", new : " + std::to_string(it->idxReelNew+it->nbIdxReelNew-1), __FILE__, __LINE__);
            outputs += "Error mismatched in start of end. Current : " + std::to_string(finalMatches.at(idxEndRef).nbIdxReelNew) + ", new : " + std::to_string(it->idxReelNew+it->nbIdxReelNew-1) + "\n";
            synchronized = false;
            return;
        }
    }

    this->matches = finalMatches;
}

void Synchro::checkFilled()
{   
    for (int i = 0; i < matches.size(); i++) {

        // -- unfilled
        if (matches.at(i).idxReelNew == -1 || matches.at(i).nbIdxReelNew == -1) {

            // -- check if start
            if (i == 0) {

                // -- detect next match
                int idxEnd = i+1;
                while (idxEnd < matches.size() && matches.at(idxEnd).idxReelNew == -1) {
                    idxEnd++;
                }
                
                // -- no match found at all : should not happen at this point
                /*if (idxEnd == matches.size()) {
                    synchronized = false;
                    continue;
                }*/

                // -- error if more than one reel before resynchro
                if (idxEnd > 1) {
                    synchronized = false;
                    Poco::Logger::get("Synchro").error("More than one REF reel do not match at begin !", __FILE__, __LINE__);

                    InterMatch inter;
                    inter.startMsMatchRefUncut = 0;
                    inter.startMsMatchNew = -1;
                    inter.endMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, nullptr);
                    inter.endMsMatchNew = idxEnd;
                    inter.valid = false;
                    interMatches.push_back(inter);
                }
                else {
                    // -- check timings
                    double durationStartRef = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, cuts);
                    double durationStartNew = newCpl->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelNew, nullptr);

                    // -- error if more than 60 seconds delta
                    if (std::abs(durationStartRef - durationStartNew) > 60000.0) {
                        synchronized = false;
                        Poco::Logger::get("Synchro").error("Delta at begin is too high !", __FILE__, __LINE__);
                        InterMatch inter;
                        inter.startMsMatchRef = 0;
                        inter.startMsMatchRefUncut = 0;
                        inter.startMsMatchNew = 0;
                        inter.endMsMatchRef = durationStartRef;
                        inter.endMsMatchNew = durationStartNew;
                        inter.endMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, nullptr);
                        inter.valid = false;
                        interMatches.push_back(inter);
                    }
                    else {
                        if (durationStartRef == durationStartNew) {
                            startDelay = 0;
                            entryPoint = 0;

                            // -- fill match at start
                            matches.at(i).idxReelNew = 0;
                            matches.at(i).nbIdxReelNew = matches.at(idxEnd).idxReelNew;
                        } 
                        else if (durationStartRef - durationStartNew > 0) {
                            startDelay = 0;
                            entryPoint = newCpl->convertToFrameFromIndexDecrement(durationStartRef - durationStartNew, matches.at(idxEnd).idxReelNew);

                            int durationStartRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, nullptr);
                            int entryPointUncut = newCpl->convertToFrameFromIndexDecrement(durationStartRefUncut - durationStartNew, matches.at(idxEnd).idxReelNew);

                            InterMatch inter;
                            inter.startMsMatchRef = 0;
                            inter.startMsMatchRefUncut = 0;
                            inter.endMsMatchRef = entryPoint;
                            inter.endMsMatchRefUncut = entryPointUncut;
                            inter.startMsMatchNew = -1;
                            inter.endMsMatchNew = -1;
                            inter.valid = true;
                            interMatches.push_back(inter);

                            // -- fill match at start
                            matches.at(i).idxReelNew = 0;
                            matches.at(i).nbIdxReelNew = matches.at(idxEnd).idxReelNew;
                        }
                        else if (durationStartNew - durationStartRef > 0) {
                            entryPoint = 0;
                            startDelay = newCpl->convertToFrameFromIndexDecrement(durationStartNew - durationStartRef, matches.at(idxEnd).idxReelNew);

                            int durationStartRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, nullptr);
                            int startDelayUncut = newCpl->convertToFrameFromIndexDecrement(durationStartNew - durationStartRefUncut, matches.at(idxEnd).idxReelNew);

                            InterMatch inter;
                            inter.startMsMatchRef = startDelay;
                            inter.startMsMatchRefUncut = -startDelayUncut;
                            inter.endMsMatchRef = 0;
                            inter.endMsMatchRefUncut = 0;
                            inter.startMsMatchNew = -1;
                            inter.endMsMatchNew = -1;
                            inter.valid = true;
                            interMatches.push_back(inter);

                            // -- fill match at start
                            matches.at(i).idxReelNew = 0;
                            matches.at(i).nbIdxReelNew = matches.at(idxEnd).idxReelNew;
                        }
                    }
                }

                i = idxEnd;
            }

            else {
                // -- detect next match
                int idxEnd = i+1;
                while (idxEnd < matches.size() && matches.at(idxEnd).idxReelNew == -1) {
                    idxEnd++;
                }
                
                // -- check with total duration at end of file
                if (idxEnd == matches.size()) {

                    Poco::Logger::get("Synchro").error("No match until the end !", __FILE__, __LINE__);

                    // -- more than one reel unsynchronized from ref reel : error 
                    if (i < matches.size() - 1) {
                        synchronized = false;
                        Poco::Logger::get("Synchro").error("More than one REF reel do match at end !", __FILE__, __LINE__);
                        InterMatch inter;
                        inter.startMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, nullptr);
                        inter.startMsMatchNew = matches.size() - i;
                        inter.endMsMatchRefUncut = synchronizable->getTotalMsDuration(nullptr);
                        inter.endMsMatchNew = -1;
                        inter.valid = false;
                        interMatches.push_back(inter);
                    }
                    else {

                        Poco::Logger::get("Synchro").error("Only one REF reel do match at end !", __FILE__, __LINE__);

                        // -- check timings
                        double durationEndRef = synchronizable->getTotalMsDurationFromIdx(matches.at(i-1).idxReelRef, cuts);
                        double durationEndNew = newCpl->getTotalMsDurationFromIdx(matches.at(i-1).nbIdxReelNew, nullptr);

                        // -- error if more than 60 seconds delta
                        if (std::abs(durationEndRef - durationEndNew) > 60000.0) {
                            synchronized = false;
                            Poco::Logger::get("Synchro").error("Delta at end is too high !", __FILE__, __LINE__);
                            InterMatch inter;
                            inter.startMsMatchRef = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, cuts);
                            inter.startMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, nullptr);
                            inter.startMsMatchNew = newCpl->getTotalMsDurationUntilIdx(matches.at(i-1).nbIdxReelNew+1, nullptr);
                            inter.endMsMatchRef = synchronizable->getTotalMsDuration(cuts);
                            inter.endMsMatchRefUncut = synchronizable->getTotalMsDuration(nullptr);
                            inter.endMsMatchNew = newCpl->getTotalMsDuration(nullptr);
                            inter.valid = false;
                            interMatches.push_back(inter);
                        }
                        else {
                            InterMatch inter;
                            inter.startMsMatchRef = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, cuts);
                            inter.startMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, nullptr);
                            inter.startMsMatchNew = newCpl->getTotalMsDurationUntilIdx(matches.at(i-1).nbIdxReelNew+1, nullptr);
                            inter.endMsMatchRef = synchronizable->getTotalMsDuration(cuts);
                            inter.endMsMatchRefUncut = synchronizable->getTotalMsDuration(nullptr);
                            inter.endMsMatchNew = newCpl->getTotalMsDuration(nullptr);
                            inter.valid = true;
                            interMatches.push_back(inter);
                        }
                    }

                    i = idxEnd;
                }

                // -- check between
                else {

                    Poco::Logger::get("Synchro").error("No match between !", __FILE__, __LINE__);

                    // -- check timings
                    double durationRef = synchronizable->getTotalMsDurationBetweenIdx(matches.at(i).idxReelRef, matches.at(idxEnd).idxReelRef-1, cuts);
                    double durationNew = newCpl->getTotalMsDurationBetweenIdx(matches.at(i-1).nbIdxReelNew+1, matches.at(idxEnd).idxReelNew-1, nullptr);

                    // -- error if more than 60 seconds delta
                    if (std::abs(durationRef - durationNew) > epsilon) {
                        synchronized = false;
                        Poco::Logger::get("Synchro").error("Delta between is too high !", __FILE__, __LINE__);
                        InterMatch inter;
                        inter.startMsMatchRef = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, cuts);
                        inter.startMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, nullptr);
                        inter.startMsMatchNew = newCpl->getTotalMsDurationUntilIdx(matches.at(i-1).nbIdxReelNew+1, nullptr);
                        inter.endMsMatchRef = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, cuts);
                        inter.endMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, nullptr);
                        inter.endMsMatchNew = newCpl->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelNew, nullptr);
                        inter.valid = false;
                        interMatches.push_back(inter);
                    }
                    else {
                        InterMatch inter;
                        inter.startMsMatchRef = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, cuts);
                        inter.startMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(i).idxReelRef, nullptr);
                        inter.startMsMatchNew = newCpl->getTotalMsDurationUntilIdx(matches.at(i-1).nbIdxReelNew+1, nullptr);
                        inter.endMsMatchRef = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, cuts);
                        inter.endMsMatchRefUncut = synchronizable->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelRef, nullptr);
                        inter.endMsMatchNew = newCpl->getTotalMsDurationUntilIdx(matches.at(idxEnd).idxReelNew, nullptr);
                        inter.valid = true;
                        interMatches.push_back(inter);
                    }

                    i = idxEnd;
                }
            }
        }
    
    }
}

SyncFile* Synchro::generateSyncFile(std::string pathFile)
{
    SyncFile* sync = new SyncFile(pathFile);
    sync->setVersion("2.0");
    sync->setType("imagesequence");
    sync->setCplId(newCpl->getCplId());
    sync->setCplTitle(newCpl->getCplTitle());
    sync->setCplDuration(newCpl->getTotalMsDuration(NULL)*newCpl->getReels().at(0).speedRate/1000.0);
    sync->setSpeedRate(newCpl->getReels().at(0).speedRate);
    sync->setEditRate(newCpl->getReels().at(0).editRate);
    sync->setTypeProjection(synchronizable->getTypeProjection());
    
    // -- set reels
    for (int i = 0; i < newCpl->getReels().size(); i++) {
        sync->addReel(newCpl->getReels().at(i));
    }

    // -- recalculation of entryPoint and startDelay
    int ep = entryPoint + synchronizable->getEntryPoint();
    int sd = startDelay + synchronizable->getStartDelay();
    if (ep > 0 && sd > 0) {
        if (ep == sd) {
            ep = 0;
            sd = 0;
        }
        else if (ep > sd) {
            ep -= sd;
            sd = 0;
        }
        else {
            sd -= ep;
            ep = 0;
        }
    }

    if (cuts == NULL) {
        cuts = new CutScenes(newCpl->getCplId(), newCpl->getCplTitle(), synchronizable->getCplId(), synchronizable->getCplTitle());
    }
    // -- offset current cuts
    else {
        cuts->shiftCuts(synchronizable->getStartDelay() - synchronizable->getEntryPoint());
    }

    // -- change phasing of cutscenes
    // -- TODO : test if correct
    if (synchronizable->getCuts() != NULL) {

        for (int i = 0; i < synchronizable->getCuts()->getListOfCuts().size(); i++) {
            CutScenes::Cut cut = synchronizable->getCuts()->getListOfCuts().at(i);
            cut.startFrame += startDelay - entryPoint;
            cut.endFrame += startDelay - entryPoint;
            cuts->addCut(cut.startFrame, cut.endFrame, cut.description);
        }
    }

    sync->setCuts(cuts);
    sync->setStartValues(sd, ep);
    
    std::vector<std::shared_ptr<Command>>::iterator it;
    for (it = synchronizable->getCommandsOnStartSync().begin(); it != synchronizable->getCommandsOnStartSync().end(); it++) {
        sync->addStartCommand(*it);
    }
    for (it = synchronizable->getCommandsOnEndSync().begin(); it != synchronizable->getCommandsOnEndSync().end(); it++) {
        sync->addEndCommand(*it);
    }

    return sync;
}

std::string Synchro::matchesToString()
{
    std::string str = "";
    for (int i = 0; i < matches.size(); i++) {
        str += "MATCH : " + std::to_string(matches.at(i).duration) + "ms\n";
        str += "REF : " + std::to_string(matches.at(i).idxReelRef) + " - " + std::to_string(matches.at(i).nbIdxReelRef) + "\n";
        str += "NEW : " + std::to_string(matches.at(i).idxReelNew) + " - " + std::to_string(matches.at(i).nbIdxReelNew) + "\n";
        str += "--------------------------------------------------\n";
    }

    return str;
}

std::string Synchro::toXmlString(bool byId)
{
    std::string xml = "<SYNCHRO>";

    xml += "<ref>";
    for (int i = 0; i < synchronizable->getReels().size(); i++) {
        Synchronizable::Reel reel = synchronizable->getReels().at(i);

        xml += "<reel>";
        xml += "<id>" + reel.id + "</id>";
        xml += "<duration>" + std::to_string(reel.duration) + "</duration>";
        xml += "<speedRate>" + std::to_string(reel.speedRate) + "</speedRate>";        
        xml += "</reel>";
    }
    xml += "</ref>";

    xml += "<new>";
    for (int i = 0; i < newCpl->getReels().size(); i++) {
        Synchronizable::Reel reel = newCpl->getReels().at(i);

        xml += "<reel>";
        xml += "<id>" + reel.id + "</id>";
        xml += "<duration>" + std::to_string(reel.duration) + "</duration>";
        xml += "<speedRate>" + std::to_string(reel.speedRate) + "</speedRate>";        
        xml += "</reel>";
    }
    xml += "</new>";

    xml += "<matches>";
    if (byId) {
        for (int i = 0; i < matches2.size(); i++) {
            Synchro::Match2 match = matches2.at(i);

            xml += "<match>";
            xml += "<startRef>" + std::to_string(match.startMsMatchRef) + "</startRef>";
            xml += "<endRef>" + std::to_string(match.endMsMatchRef) + "</endRef>";
            xml += "<startNew>" + std::to_string(match.startMsMatchNew) + "</startNew>";
            xml += "<endNew>" + std::to_string(match.endMsMatchNew) + "</endNew>";        
            xml += "</match>";
        }
    }
    else {
        for (int i = 0; i < matches.size(); i++) {
            Synchro::Match match = matches.at(i);

            xml += "<match>";
            xml += "<idxStartRef>" + std::to_string(match.idxReelRef) + "</idxStartRef>";
            xml += "<nbReelRef>" + std::to_string(match.nbIdxReelRef) + "</nbReelRef>";
            xml += "<idxStartNew>" + std::to_string(match.idxReelNew) + "</idxStartNew>";
            xml += "<idxEndNew>" + std::to_string(match.nbIdxReelNew) + "</idxEndNew>";        
            xml += "</match>";
        }
    }
    xml += "</matches>";

    xml += "<interMatches>";
    for (int i = 0; i < interMatches.size(); i++) {
        Synchro::InterMatch inter = interMatches.at(i);

        xml += "<inter>";
        xml += "<startRef>" + std::to_string(inter.startMsMatchRef) + "</startRef>";
        xml += "<startRefUncut>" + std::to_string(inter.startMsMatchRefUncut) + "</startRefUncut>";
        xml += "<endRef>" + std::to_string(inter.endMsMatchRef) + "</endRef>";
        xml += "<endRefUncut>" + std::to_string(inter.endMsMatchRefUncut) + "</endRefUncut>";
        xml += "<startNew>" + std::to_string(inter.startMsMatchNew) + "</startNew>";
        xml += "<endNew>" + std::to_string(inter.endMsMatchNew) + "</endNew>";
        xml += "<valid>" + std::to_string(inter.valid) + "</valid>";
        xml += "</inter>";
    }
    xml += "</interMatches>";

    xml += "<invalidCuts>";
    if (byId) {
        for (int i = 0; i < invalidCuts.size(); i++) {
            Synchro::InvalidCut cut = invalidCuts.at(i);

            xml += "<inter>";
            xml += "<start>" + std::to_string(cut.startMs) + "</start>";
            xml += "<end>" + std::to_string(cut.endMs) + "</end>";
            xml += "<left>" + std::to_string(cut.left) + "</left>";
            xml += "<right>" + std::to_string(cut.right) + "</right>";
            xml += "<outLeft>" + std::to_string(cut.nbFrameOutLeft) + "</outLeft>";
            xml += "<outRight>" + std::to_string(cut.nbFrameOutRight) + "</outRight>";
            xml += "<nbMissing>" + std::to_string(cut.nbMissingFrame) + "</nbMissing>";
            xml += "</inter>";
        }
    }
    xml += "</invalidCuts>";

    xml += "</SYNCHRO>";
    return xml;
}