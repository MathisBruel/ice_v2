#include "Domain/Content.h"
#include "Domain/ContentRepo.h"
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
void Content::setDatas(int id, std::string title)
{
    this->id = id;
    this->title = title;
}
void Content::setRelease(Releases* release)
{
    this->release = release;
}
void Content::CreateRelease(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie){
    this->release = new Releases();
    this->release->setId(id_movie, typeMovie, localisationMovie);
}