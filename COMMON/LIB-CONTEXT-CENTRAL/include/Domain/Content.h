#pragma once
#include "ContentRepo.h"
#include "Release.h"
#include <iostream>

class Content
{
public:
    Content();
    Content(std::string title);
    ~Content();

    void CreateRelease(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    
    void setDatas(std::string title);
    void setRelease(Release* release);

    std::string getTitle() {return this->title;}
    Release* getRelease() {return this->release;}
private:
    ContentRepo* contentRepo;
    Release* release;

    std::string title;
};