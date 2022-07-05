#include <cstdlib>
#include <iostream>

#pragma once
#include "commandResponse.h"
#include "synchro.h"

class RepositoryResponse : public CommandResponse
{

public:

    RepositoryResponse(std::string uuidFromCommand, Command::CommandType type);

    void setReelsCplNew(std::vector<Synchronizable::Reel> reelsCplNew) {this->reelsCplNew = reelsCplNew;}
    void setReelsSyncRef(std::vector<Synchronizable::Reel> reelsSyncRef) {this->reelsSyncRef = reelsSyncRef;}
    void setMatchesByDuration(std::vector<Synchro::Match> matchesByDuration) {this->matchesByDuration = matchesByDuration;}
    void setMatchesById(std::vector<Synchro::Match2> matchesById) {this->matchesById = matchesById;}
    void setInterMatches(std::vector<Synchro::InterMatch> interMatches) {this->interMatches = interMatches;}
    void setInvalidCuts(std::vector<Synchro::InvalidCut> invalidCuts) {this->invalidCuts = invalidCuts;}
    void setById(bool typeIsById) {byId = typeIsById;}
    void setContentSync(std::string contentSync) {this->contentSync = contentSync;}

    ~RepositoryResponse();

    void fillDatas();

private:

    void createOutputSynchroXml(Poco::XML::Document* doc);
    void createDownloadSyncXml(Poco::XML::Document* doc);

    std::vector<Synchronizable::Reel> reelsCplNew; 
    std::vector<Synchronizable::Reel> reelsSyncRef;
    std::vector<Synchro::Match> matchesByDuration;
    std::vector<Synchro::Match2> matchesById;
    std::vector<Synchro::InterMatch> interMatches;
    std::vector<Synchro::InvalidCut> invalidCuts;

    bool byId;
    std::string contentSync;
};