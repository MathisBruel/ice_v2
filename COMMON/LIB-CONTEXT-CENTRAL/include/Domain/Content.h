#pragma once

#include <map>
#include <iostream>

#include "Domain/Release.h"


class StateMachine;         
class StateMachineManager;  
class ContentRepo;

class Content
{
public:
    Content();
    Content(std::string contentTitle);

    void CreateRelease(int id_content, int typeMovie, int localisationMovie);
    
    void SetContentId(int contentId);
    void SetDatas(std::string contentTitle);
    void AddRelease(Releases* release);
    void DeleteRelease(std::string releaseId);
    void SetStateMachine(StateMachine* stateMachine);

    std::string toXmlString(bool printChild);   

    int* GetContentId() {return &this->_contentId;}
    std::string GetContentTitle() {return this->_contentTitle;}
    std::map<std::string, Releases*> GetReleases() {return this->_release;}
    Releases* GetRelease(std::string releaseId) {return this->_release[releaseId];}

private:
    ContentRepo* _contentRepo;
    std::map<std::string, Releases*> _release;
    int _contentId;
    std::string _contentTitle;
    StateMachine* _stateMachine;
};