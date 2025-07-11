#pragma once
class PublishingInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStatePublishing;
    std::function<void()> pfTransitionToReleaseCreation;
    std::function<void()> pfTransitionToCancel;
};