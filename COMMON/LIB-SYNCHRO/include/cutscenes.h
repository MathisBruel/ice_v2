#include <cstdlib>
#include <iostream>
#include <vector>

#pragma once

class CutScenes
{

public:

    struct Cut
    {
        int startFrame;     // -- included in the cut
        int endFrame;       // -- included in the cut
        std::string description;
    };

    CutScenes(std::string cplIdCut, std::string cplTitleCut, std::string cplIdRef, std::string cplTitleRef);
    ~CutScenes();

    void addCut(int startFrame, int endFrame, std::string description);
    void replaceCut(int startFrame, int endFrame, std::string description, int index);
    void removeCut(int index);
    bool loadFromStringXmlContent(std::string content);

    std::string getCplIdCut() {return cplIdCut;}
    std::string getCplTitleCut() {return cplTitleCut;}
    std::string getCplIdRef() {return cplIdRef;}
    std::string getCplTitleRef() {return cplTitleRef;}

    std::vector<Cut> getListOfCuts() {return listCutScenes;}

    void shiftCuts(int nbFrame);

private:

    std::string cplIdCut;
    std::string cplTitleCut;
    std::string cplIdRef;
    std::string cplTitleRef;

    std::vector<Cut> listCutScenes;
};