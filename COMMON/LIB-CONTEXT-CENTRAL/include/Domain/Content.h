#pragma once
#include "Release.h"
#include <map>
#include <iostream>
class StateMachine;         
class StateMachineManager;  
#include "App/Context.h"
#include "Infra/HTTPContentInteraction.h"
#include "Infra/HTTPPublishingInteraction.h"
#include "Infra/HTTPReleaseInteraction.h"
#include "Infra/HTTPCISInteraction.h"
#include "Infra/HTTPIdleSyncInteraction.h"
#include "Infra/HTTPCPLInteraction.h"
#include "Infra/HTTPSyncInteraction.h"
#include "Infra/HTTPSyncLoopInteraction.h"
#include "Infra/HTTPInProdInteraction.h"
#include "Infra/MySQLDBConnection.h"


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
    StateMachine* CreateStateMachine(int contentId, MySQLDBConnection* dbConnection);

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