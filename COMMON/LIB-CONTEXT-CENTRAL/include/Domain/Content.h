#pragma once
#include "Release.h"
#include <iostream>

class ContentRepo;
class Content
{
public:
    Content();
    Content(std::string contentTitle);
    ~Content();

    void CreateRelease(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    
    void SetDatas(int contentId, std::string contentTitle);
    void SetRelease(Releases* release);

    int* GetContentId() {return &this->_contentId;}
    std::string GetContentTitle() {return this->_contentTitle;}
    Releases* GetRelease() {return this->_release;}
    
private:
    ContentRepo* _contentRepo;
    Releases* _release;
    int _contentId;
    std::string _contentTitle;
};