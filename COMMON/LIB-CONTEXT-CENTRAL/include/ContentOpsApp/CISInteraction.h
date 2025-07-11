#pragma once
class CISInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateUploadCIS;
    std::function<void()> pfTransitionToInProduction;
};