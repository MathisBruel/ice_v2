#include "Domain/Content.h"
#include "Domain/ContentRepo.h"
Content::Content()
{
    _contentId = -1;
    _contentTitle = "";
}
Content::Content(std::string contentTitle)
{
    _contentId = -1;
    this->_contentTitle = contentTitle;
}
void Content::SetContentId(int contentId)
{
    this->_contentId = contentId;
}
void Content::SetDatas(std::string contentTitle)
{
    this->_contentTitle = contentTitle;
}
void Content::AddRelease(Releases* release)
{
    std::string releaseId = std::to_string(release->GetReleaseId()[0]) + "_" +
                            std::to_string(release->GetReleaseId()[1]) + "_" +
                            std::to_string(release->GetReleaseId()[2]);
    this->_release[releaseId] = release;
}
void Content::DeleteRelease(std::string releaseId)
{
    delete this->_release[releaseId];
    this->_release[releaseId] = nullptr;
}
void Content::CreateRelease(int id_movie, int typeMovie, int localisationMovie){
    TypeMovie idTypeMovie = static_cast<TypeMovie>(typeMovie);
    LocalisationMovie idLocalisationMovie = static_cast<LocalisationMovie>(localisationMovie);

    std::string releaseId = std::to_string(id_movie) + "_" +
                        std::to_string(typeMovie) + "_" +
                        std::to_string(localisationMovie);
    this->_release[releaseId] = new Releases();
    this->_release[releaseId]->SetReleaseId(id_movie, idTypeMovie, idLocalisationMovie);
}

std::string Content::toXmlString(bool printChild)
{
    std::string xml = "<content";
    xml += " id_movie=\"" + std::to_string(this->_contentId) + "\"";
    xml += " title=\"" + this->_contentTitle + "\"";
    if (printChild) {
        xml += ">"; 
        for (auto const& release : this->_release) {
            xml += release.second->toXmlString(false);
        }
    }
    else {
        xml += "/>";
    }
    return xml;
}