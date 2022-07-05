#include "xml/PlayerStatus.h"

PlayerStatus::PlayerStatus()
{
    state = UnknownState;
    
    cplId = "Not known";
    cplTitle = "Not known";

    speedRate = 24;
    editPosition = 0;
}

PlayerStatus::PlayerStatus(PlayerStatus &player)
{
    state = player.state;
    cplId = player.cplId;
    cplTitle = player.cplTitle;
    speedRate = player.speedRate;
    editPosition = player.editPosition;
    timestamp = player.timestamp;
}

PlayerStatus::~PlayerStatus(){}

double PlayerStatus::getTimeCodeMs()
{
    if (state == UnknownState || state == Stop || speedRate == 0.0) {
        return 0;
    }
    return (double)editPosition*1000.0 / speedRate + 0.5;
}

PlayerStatus::State PlayerStatus::parseStateFromString(std::string state)
{
    if (state == "Pause") {
        return Pause;
    }
    else if (state == "Play") {
        return Play;
    }
    else if (state == "Stop") {
        return Stop;
    }
    else {
        return UnknownState;
    }
}

std::string PlayerStatus::getStateToString()
{
    if (state == Pause) {
        return "Pause";
    }
    else if (state == Play) {
        return "Play";
    }
    else if (state == Stop) {
        return "Stop";
    }
    else {
        return "Unknown";
    }
}

std::string PlayerStatus::toString()
{
    std::string data = "";
    data += " -- Player status --\n";
    data += "Cpl id : " + cplId + "\n";
    data += "Cpl title : " + cplTitle + "\n";
    data += "Speed rate : " + std::to_string(speedRate) + "\n";
    data += "Edit position : " + std::to_string(editPosition) + "\n";
    data += "State : " + getStateToString() + "\n";
    data += "\n";

    return data;
}