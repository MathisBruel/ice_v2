#include <cstdlib>
#include <iostream>
#include <queue>
#include "Template.h"
#include "TemplateV1.h"
#include "MovingHeadProjection.h"
#include "PanelProjection.h"

#pragma once 

class FrameOutputs {

public:

    FrameOutputs(int currentFrame);
    ~FrameOutputs();

    void addPanelOutput(std::shared_ptr<OutputPanelImage> output);
    void addMovingHeadOutput(std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> output);
    void addParOutput(std::shared_ptr<Template::ParOutputs> output);
    void addBacklightOutput(std::shared_ptr<Template::BacklightOutputs> output);

    int getNbPanelOutput() {return panels.size();}
    std::shared_ptr<OutputPanelImage> getPanelOutput(int index) {return panels.at(index);}
    int getNbMovingsOutput() {return movings.size();}
    std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> getMovingHeadOutput(int index) {return movings.at(index);}
    int getNbParOutput() {return pars.size();}
    std::shared_ptr<Template::ParOutputs> getParOutput(int index) {return pars.at(index);}
    int getNbBacklightOutput() {return backlights.size();}
    std::shared_ptr<Template::BacklightOutputs> getBacklightOutput(int index) {return backlights.at(index);}

    void setComplete() {complete = true;}
    bool isComplete() {return complete;}
    void setPlayed() {played = true;}
    bool isAlreadyPlayed() {return played;}

    int64_t getTimestamp() {return timestamp;}

private:


    int currentFrame;
    // -- changed by many threads but read by only one
    std::vector<std::shared_ptr<OutputPanelImage>> panels;
    // -- change by 1 thread at the time
    std::vector<std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle>> movings;
    std::vector<std::shared_ptr<Template::ParOutputs>> pars;
    std::vector<std::shared_ptr<Template::BacklightOutputs>> backlights;
    bool complete;
    bool played;

    int64_t timestamp;
};