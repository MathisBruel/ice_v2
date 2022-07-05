#include "FrameOutputs.h"

FrameOutputs::FrameOutputs(int currentFrame)
{
    timestamp = Poco::Timestamp().epochMicroseconds();
    this->currentFrame = currentFrame;
    complete = false;
    played = false;
}
FrameOutputs::~FrameOutputs()
{
    for (std::shared_ptr<OutputPanelImage> panel : panels) {
        panel.reset();
    }
    panels.clear();
    for (std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> mh : movings) {
        mh.reset();
    }
    movings.clear();
    for (std::shared_ptr<Template::ParOutputs> par : pars) {
        par.reset();
    }
    pars.clear();
    for (std::shared_ptr<Template::BacklightOutputs> backlight : backlights) {
        backlight.reset();
    }
    backlights.clear();
}

void FrameOutputs::addPanelOutput(std::shared_ptr<OutputPanelImage> output)
{
    panels.push_back(output);
}
void FrameOutputs::addMovingHeadOutput(std::shared_ptr<MovingHeadProjection::OutputMovingHeadAngle> output)
{
    movings.push_back(output);
}
void FrameOutputs::addParOutput(std::shared_ptr<Template::ParOutputs> output)
{   
    pars.push_back(output);
}
void FrameOutputs::addBacklightOutput(std::shared_ptr<Template::BacklightOutputs> output)
{
    backlights.push_back(output);
}