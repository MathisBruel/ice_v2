#include "Content.h"

Content::Content()
{
    title = "";
    release = nullptr;
}
Content::~Content() {
    if (release != nullptr) {
        delete release;
    }
}
Content::Content(std::string title)
{
    this->title = title;
    release = nullptr;
}
Content::setDatas(std::string title)
{
    this->title = title;
}
Content::setRelease(Release* release)
{
    this->release = release;
}
Content::CreateRelease(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie){
    this->release = new Release();
    this->release->setId(id_movie, typeMovie, localisationMovie);
}