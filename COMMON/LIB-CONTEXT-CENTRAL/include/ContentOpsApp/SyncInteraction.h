#pragma once
class SyncInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateSync;
    std::function<void()> pfTransitionToInProduction;
};