#include "synchronizable.h"

Synchronizable::Synchronizable(std::string pathFile)
{
    this->pathFile = pathFile;
    cuts = nullptr;
    typeProjection = PanelProjection::UNKNOWN;
    speedRate = -1.0;
    startDelay = 0.0;
    entryPoint = 0.0;
}

Synchronizable::~Synchronizable()
{

}

void Synchronizable::addStartCommand(std::shared_ptr<Command> cmd)
{
    commandsOnStartSync.push_back(cmd);
}
void Synchronizable::addEndCommand(std::shared_ptr<Command> cmd)
{
    commandsOnEndSync.push_back(cmd);
}

double Synchronizable::getTotalMsDuration(CutScenes* cuts)
{
    double totalDuration = 0;
    std::vector<Reel>::iterator it;
    int framesCounter = 0;

    double speedRateFinal = speedRate;

    for (it = reels.begin(); it != reels.end(); it++) {

        if (speedRate == -1.0) {
            speedRateFinal = it->speedRate;
        }

        totalDuration += (double)(it->duration)*1000.0/speedRateFinal;
        
        // -- treat cuts
        if (cuts != nullptr) {
            for (int j = 0; j < cuts->getListOfCuts().size(); j++) {
                
                CutScenes::Cut& cut = cuts->getListOfCuts().at(j);

                // -- cut inside reel
                if (cut.startFrame >= framesCounter && cut.endFrame < framesCounter+it->duration) {
                    totalDuration -= (double)((cut.endFrame - cut.startFrame + 1))*1000.0/speedRateFinal;
                }

                // -- cut part at end of reel
                else if (cut.startFrame >= framesCounter && cut.endFrame >= framesCounter+it->duration && cut.startFrame < framesCounter+it->duration) {
                    totalDuration -= (double)((framesCounter+it->duration - cut.startFrame))*1000.0/speedRateFinal;
                }

                // -- cut part at begin of reel
                else if (cut.startFrame < framesCounter && cut.endFrame < framesCounter+it->duration && cut.endFrame >= framesCounter) {
                    totalDuration -= (double)((cut.endFrame - framesCounter))*1000.0/speedRateFinal;
                }
            }
        }
        framesCounter += it->duration;
    }
    return totalDuration;
}

int Synchronizable::convertToFrameFromIndexDecrement(double time, int endIdx)
{
    if (endIdx < 0 || endIdx >= reels.size()) {
        return -1;
    }

    int duration = 0;
    for (int i = endIdx-1; i >= 0; i--) {

        if (time < (double)reels.at(i).duration*1000.0/reels.at(i).speedRate) {
            duration += time*reels.at(i).speedRate/1000.0;
            break;
        }
        else {
            duration += reels.at(i).duration;
            time -= (double)reels.at(i).duration*1000.0/reels.at(i).speedRate;
        }
    }

    return duration;
}

double Synchronizable::getTotalMsDurationUntilIdx(int idx, CutScenes* cuts)
{
    if (idx < 0 || idx >= reels.size()) {
        return -1;
    }

    double durationMs = 0;
    int framesCounter = 0;
    double speedRateFinal = speedRate;

    for (int i = 0; i < idx; i++) {

        if (speedRate == -1.0) {
            speedRateFinal = reels.at(i).speedRate;
        }

        durationMs += (double)(reels.at(i).duration)*1000.0/speedRateFinal;

        // -- treat cuts
        if (cuts != nullptr) {
            for (int j = 0; j < cuts->getListOfCuts().size(); j++) {
                
                CutScenes::Cut& cut = cuts->getListOfCuts().at(j);

                // -- cut inside reel
                if (cut.startFrame >= framesCounter && cut.endFrame < framesCounter+reels.at(i).duration) {
                    durationMs -= ((double)(cut.endFrame - cut.startFrame + 1))*1000.0/speedRateFinal;
                }

                // -- cut part at end of reel
                else if (cut.startFrame >= framesCounter && cut.endFrame >= framesCounter+reels.at(i).duration && cut.startFrame < framesCounter+reels.at(i).duration) {
                    durationMs -= ((double)(framesCounter+reels.at(i).duration - cut.startFrame))*1000.0/speedRateFinal;
                }

                // -- cut part at begin of reel
                else if (cut.startFrame < framesCounter && cut.endFrame < framesCounter+reels.at(i).duration && cut.endFrame >= framesCounter) {
                    durationMs -= ((double)(cut.endFrame - framesCounter))*1000.0/speedRateFinal;
                }
            }
        }
        framesCounter += reels.at(i).duration;
    }

    return durationMs;
}
double Synchronizable::getTotalMsDurationFromIdx(int idx, CutScenes* cuts)
{
    if (idx < 0 || idx >= reels.size()) {
        return -1;
    }

    double durationMs = 0;
    int framesCounter = 0;

    for (int i = 0; i < reels.size(); i++) {

        if (i >= idx+1) {
            durationMs += (double)(reels.at(i).duration)*1000.0/reels.at(i).speedRate;

            // -- treat cuts
            if (cuts != nullptr) {
                for (int j = 0; j < cuts->getListOfCuts().size(); j++) {
                    
                    CutScenes::Cut& cut = cuts->getListOfCuts().at(j);

                    // -- cut inside reel
                    if (cut.startFrame >= framesCounter && cut.endFrame < framesCounter+reels.at(i).duration) {
                        durationMs -= (double)(cut.endFrame - cut.startFrame + 1)*1000.0/reels.at(i).speedRate;
                    }

                    // -- cut part at end of reel
                    else if (cut.startFrame >= framesCounter && cut.endFrame >= framesCounter+reels.at(i).duration && cut.startFrame < framesCounter+reels.at(i).duration) {
                        durationMs -= (double)(framesCounter+reels.at(i).duration - cut.startFrame)*1000.0/reels.at(i).speedRate;
                    }

                    // -- cut part at begin of reel
                    else if (cut.startFrame < framesCounter && cut.endFrame < framesCounter+reels.at(i).duration && cut.endFrame >= framesCounter) {
                        durationMs -= (double)(cut.endFrame - framesCounter)*1000.0/reels.at(i).speedRate;
                    }
                }
            }
        }
        framesCounter += reels.at(i).duration;
    }

    return durationMs;
}
double Synchronizable::getTotalMsDurationBetweenIdx(int idxStart, int idxEnd, CutScenes* cuts)
{
    if (idxStart < 0 || idxStart >= reels.size() || idxEnd < 0 || idxEnd >= reels.size()) {
        return -1;
    }

    double durationMs = 0;
    int framesCounter = 0;

    for (int i = 0; i < reels.size(); i++) {

        if (i >= idxStart && i <= idxEnd) {
            durationMs += (double)(reels.at(i).duration)*1000.0/reels.at(i).speedRate;

            // -- treat cuts
            if (cuts != nullptr) {
                for (int j = 0; j < cuts->getListOfCuts().size(); j++) {
                    
                    CutScenes::Cut& cut = cuts->getListOfCuts().at(j);

                    // -- cut inside reel
                    if (cut.startFrame > framesCounter && cut.endFrame < framesCounter+reels.at(i).duration) {
                        durationMs -= (double)(cut.endFrame - cut.startFrame + 1)*1000.0/reels.at(i).speedRate;
                    }

                    // -- cut part at end of reel
                    else if (cut.startFrame > framesCounter && cut.endFrame >= framesCounter+reels.at(i).duration && cut.startFrame < framesCounter+reels.at(i).duration) {
                        durationMs -= (double)(framesCounter+reels.at(i).duration - cut.startFrame)*1000.0/reels.at(i).speedRate;
                    }

                    // -- cut part at begin of reel
                    else if (cut.startFrame <= framesCounter && cut.endFrame < framesCounter+reels.at(i).duration && cut.endFrame >= framesCounter) {
                        durationMs -= (double)(cut.endFrame - framesCounter)*1000.0/reels.at(i).speedRate;
                    }
                }
            }
        }
        framesCounter += reels.at(i).duration;
    }

    return durationMs;
}
