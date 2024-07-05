#pragma once
#include "Release.h"
#include <map>
#include <iostream>

class ContentRepo;
class Content
{
public:
    Content();
    Content(std::string contentTitle);
    ~Content();

    void CreateRelease(int id_movie, int typeMovie, int localisationMovie);
    
    void SetContentId(int contentId);
    void SetDatas(std::string contentTitle);
    void AddRelease(Releases* release);
    std::string toXmlString();   

    int* GetContentId() {return &this->_contentId;}
    std::string GetContentTitle() {return this->_contentTitle;}
    std::map<std::string, Releases*> GetReleases() {return this->_release;}
    Releases* GetRelease(std::string releaseId) {return this->_release[releaseId];}
    
private:
    ContentRepo* _contentRepo;
    std::map<std::string, Releases*> _release;
    int _contentId;
    std::string _contentTitle;
};