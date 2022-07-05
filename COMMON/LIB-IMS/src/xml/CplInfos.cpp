#include "xml/CplInfos.h"

CplInfos::CplInfos()
{
    cplId = "Unknow";
    cplTitle = "Unknow";
    contentVersionId = "Unknow";

    playable = false;
    is3D = false;
    
    speedRate = 24.0;
    cplDuration = 0;

    contentFile = "";

    pictureWidth = -1;
}
    
CplInfos::~CplInfos()
{
    
}

std::string CplInfos::toString()
{
    std::string data = "";
    data += " -- Cpl infos --\n";
    data += "Cpl id : " + cplId + "\n";
    data += "Cpl title : " + cplTitle + "\n";
    data += "Content version id : " + contentVersionId + "\n";
    data += "Speed rate : " + std::to_string(speedRate) + "\n";
    data += "Duration : " + std::to_string(cplDuration) + "\n";
    data += "Type : " + CplFile::cplTypeToString(type) + "\n";
    data += "Playable : " + std::to_string(playable) + "\n";
    data += "3D : " + std::to_string(is3D) + "\n";
    data += "\n";

    return data;
}