#include "ContentOpsDomain/COD_Content.h"
#include "ContentOpsDomain/COD_ContentRepo.h"
#include "ContentOpsApp/StateMachine.h"
#include "ContentOpsApp/StateMachineManager.h"

COD_Content::COD_Content()
{
    _contentId = -1;
    _contentTitle = "";
    _stateMachine = nullptr;

}
COD_Content::COD_Content(std::string contentTitle)
{
    _contentId = -1;
    this->_contentTitle = contentTitle;
}
void COD_Content::SetContentId(int contentId)
{
    this->_contentId = contentId;
}
void COD_Content::SetDatas(std::string contentTitle)
{
    this->_contentTitle = contentTitle;
}
void COD_Content::AddRelease(COD_Releases* release)
{
    std::string releaseId = std::to_string(release->GetReleaseId()[0]) + "_" +
                            std::to_string(release->GetReleaseId()[1]) + "_" +
                            std::to_string(release->GetReleaseId()[2]);
    this->_release[releaseId] = release;
}
void COD_Content::DeleteRelease(std::string releaseId)
{
    delete this->_release[releaseId];
    this->_release[releaseId] = nullptr;
}

void COD_Content::SetStateMachine(StateMachine* stateMachine)
{
    this->_stateMachine = stateMachine;
}

void COD_Content::CreateRelease(int id_content, int typeMovie, int localisationMovie){
    TypeMovie idTypeMovie = static_cast<TypeMovie>(typeMovie);
    LocalisationMovie idLocalisationMovie = static_cast<LocalisationMovie>(localisationMovie);

    std::string releaseId = std::to_string(id_content) + "_" +
                        std::to_string(typeMovie) + "_" +
                        std::to_string(localisationMovie);
    this->_release[releaseId] = new COD_Releases();
    this->_release[releaseId]->SetReleaseId(id_content, idTypeMovie, idLocalisationMovie);
}

std::string COD_Content::toXmlString(bool printChild)
{
    std::string xml = "<content";
    xml += " id_content=\"" + std::to_string(this->_contentId) + "\"";
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