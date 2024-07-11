#pragma once
#include "App/State.h"
class CISInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateUploadCIS;
};