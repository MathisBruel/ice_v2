#pragma once
#include "ContentOpsApp/State.h"
class InProdInteraction
{
public:
    std::function<TransitionResponse(std::string, std::map<std::string, std::string>)> pfStateInProd;
};