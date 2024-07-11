#pragma once
#include <string>

struct TransitionResponse {
    std::string cmdUUID;
    std::string cmdComment;
    std::string cmdStatus;
    std::string cmdDatasXML;
};