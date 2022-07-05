#include "cutscenes.h"

 CutScenes::CutScenes(std::string cplIdCut, std::string cplTitleCut, std::string cplIdRef, std::string cplTitleRef)
{
    this->cplIdCut = cplIdCut;
    this->cplTitleCut = cplTitleCut;
    this->cplIdRef = cplIdRef;
    this->cplTitleRef = cplTitleRef;
}
    
CutScenes::~CutScenes()
{
    
}

void CutScenes::addCut(int startFrame, int endFrame, std::string description)
{
    Cut cut;
    cut.startFrame = startFrame;
    cut.endFrame = endFrame;
    cut.description = description;

    listCutScenes.push_back(cut);
}

void CutScenes::replaceCut(int startFrame, int endFrame, std::string description, int index)
{
    listCutScenes.at(index).startFrame = startFrame;
    listCutScenes.at(index).endFrame = endFrame;
    listCutScenes.at(index).description = description;
}

void CutScenes::removeCut(int index)
{
    std::vector<Cut>::iterator it = listCutScenes.begin();
    std::advance(it, index);
    listCutScenes.erase(it);
}

bool CutScenes::loadFromStringXmlContent(std::string content)
{
    return true;
}

void CutScenes::shiftCuts(int nbFrame)
{
    for (int i = 0; i < listCutScenes.size(); i++) {
        listCutScenes.at(i).startFrame += nbFrame;
        listCutScenes.at(i).endFrame += nbFrame;
    }
}