#pragma once
#include "Release.h"
#include <iostream>

class ContentRepo;
class Content
{
public:
    Content();
    Content(std::string title);
    ~Content();

    void CreateRelease(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    
    void setDatas(int id, std::string title);
    void setRelease(Releases* release);

    int* getId() {return &this->id;}
    std::string getTitle() {return this->title;}
    Releases* getRelease() {return this->release;}
    
private:
    ContentRepo* contentRepo;
    Releases* release;
    int id;
    std::string title;
};