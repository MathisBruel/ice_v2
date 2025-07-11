#pragma once
class CPLInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string,std::string>)> pfStateCPL;
    std::function<void()> pfTransitionToSync;
};