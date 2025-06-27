#pragma once

#include <map>
#include <iostream>

#include "ContentOpsDomain/COD_Release.h"


class StateMachine;         
class StateMachineManager;  
class ContentRepo;

class COD_Content
{
public:
    COD_Content();
    COD_Content(std::string contentTitle);

    void CreateRelease(int id_content, int typeMovie, int localisationMovie);
    
    void SetContentId(int contentId);
    void SetDatas(std::string contentTitle);
    void AddRelease(COD_Releases* release);
    void DeleteRelease(std::string releaseId);
    void SetStateMachine(StateMachine* stateMachine);

    std::string toXmlString(bool printChild);   

    int* GetContentId() {return &this->_contentId;}
    std::string GetContentTitle() {return this->_contentTitle;}
    std::map<std::string, COD_Releases*> GetReleases() {return this->_release;}
    COD_Releases* GetRelease(std::string releaseId) {return this->_release[releaseId];}

private:
    ContentRepo* _contentRepo;
    std::map<std::string, COD_Releases*> _release;
    int _contentId;
    std::string _contentTitle;
    StateMachine* _stateMachine;
};