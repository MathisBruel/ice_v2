#include "Domain/Content.h"
#include "Domain/ContentRepo.h"
Content::Content()
{
    _contentTitle = "";
    _release = nullptr;
}
Content::~Content() {
    if (_release != nullptr) {
        delete _release;
    }
}
Content::Content(std::string contentTitle)
{
    this->_contentTitle = contentTitle;
    _release = nullptr;
}
void Content::SetDatas(int contentId, std::string contentTitle)
{
    this->_contentId = contentId;
    this->_contentTitle = contentTitle;
}
void Content::SetRelease(Releases* release)
{
    this->_release = release;
}
void Content::CreateRelease(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie){
    this->_release = new Releases();
    this->_release->SetReleaseId(id_movie, typeMovie, localisationMovie);
}