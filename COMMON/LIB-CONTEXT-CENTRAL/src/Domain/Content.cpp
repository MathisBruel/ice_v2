#include "Domain/Content.h"
#include "Domain/ContentRepo.h"
#include "App/StateMachine.h"           
#include "App/StateMachineManager.h"    
Content::Content()
{
    _contentId = -1;
    _contentTitle = "";
    _stateMachine = nullptr;

}
Content::Content(std::string contentTitle)
{
    _contentId = -1;
    this->_contentTitle = contentTitle;
    //this->_stateMachine = Configurator::CreateStateMachineForContent(-1); // -1 as placeholder, will be set later
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

void Content::SetStateMachine(StateMachine* stateMachine)
{
    this->_stateMachine = stateMachine;
}

void Content::CreateRelease(int id_content, int typeMovie, int localisationMovie){
    TypeMovie idTypeMovie = static_cast<TypeMovie>(typeMovie);
    LocalisationMovie idLocalisationMovie = static_cast<LocalisationMovie>(localisationMovie);

    std::string releaseId = std::to_string(id_content) + "_" +
                        std::to_string(typeMovie) + "_" +
                        std::to_string(localisationMovie);
    this->_release[releaseId] = new Releases();
    this->_release[releaseId]->SetReleaseId(id_content, idTypeMovie, idLocalisationMovie);
}

std::string Content::toXmlString(bool printChild)
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

StateMachine* Content::CreateStateMachine(int contentId, MySQLDBConnection* dbConnection) {
    // Création d'un nouveau contexte pour le Content
    Context* contentContext = new Context(
        new HTTPContentInteraction(),
        new HTTPPublishingInteraction(),
        new HTTPReleaseInteraction(),
        new HTTPCISInteraction(),
        new HTTPIdleSyncInteraction(),
        new HTTPCPLInteraction(),
        new HTTPSyncInteraction(),
        new HTTPSyncLoopInteraction(),
        new HTTPInProdInteraction(),
        dbConnection
    );

    // Création de la StateMachine pour ce Content
    StateMachine* contentStateMachine = new StateMachine(contentContext);
    
    // Enregistrement dans le StateMachineManager
    StateMachineManager::GetInstance()->AddStateMachine(contentId, contentStateMachine);
    
    return contentStateMachine;
}