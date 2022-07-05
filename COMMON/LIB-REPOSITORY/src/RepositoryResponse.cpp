#include "RepositoryResponse.h"

RepositoryResponse::RepositoryResponse(std::string uuidFromCommand, Command::CommandType type)
    : CommandResponse(uuidFromCommand, type)
{

}

RepositoryResponse::~RepositoryResponse()
{

}

void RepositoryResponse::fillDatas()
{
    Poco::AutoPtr<Poco::XML::Document> doc = new Poco::XML::Document;

    bool setXml = false;
    if (type == Command::CommandType::SYNCHRONIZE_SCRIPT) {
        createOutputSynchroXml(doc);
        setXml = true;
    }
    if (status == CommandResponse::OK && type == Command::CommandType::GET_SYNC_CONTENT) {
        createDownloadSyncXml(doc);
        setXml = true;
    }
    else {
        // -- NOTHING TO DO
    }

    if (setXml) {
        // -- create string XML
        std::ostringstream outStr;
        Poco::XML::DOMWriter builder;
        builder.setNewLine("\n");
        builder.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
        builder.writeNode(outStr, doc);
        datas = outStr.str();
    }

    doc->release();
}

void RepositoryResponse::createOutputSynchroXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("SYNCHRO");
    doc->appendChild(root);

    Poco::AutoPtr<Poco::XML::Element> refSync = doc->createElement("ref");
    for (int i = 0; i < reelsSyncRef.size(); i++) {
        Synchronizable::Reel reel = reelsSyncRef.at(i);
        Poco::AutoPtr<Poco::XML::Element> reelXml = doc->createElement("reel");

        Poco::AutoPtr<Poco::XML::Element> idReelXml = doc->createElement("id");
        Poco::AutoPtr<Poco::XML::Text> idReel = doc->createTextNode(reel.id);
        idReelXml->appendChild(idReel);
        reelXml->appendChild(idReelXml);

        Poco::AutoPtr<Poco::XML::Element> durationReelXml = doc->createElement("duration");
        Poco::AutoPtr<Poco::XML::Text> durationReel = doc->createTextNode(std::to_string(reel.duration));
        durationReelXml->appendChild(durationReel);
        reelXml->appendChild(durationReelXml);

        Poco::AutoPtr<Poco::XML::Element> speedRateReelXml = doc->createElement("speedRate");
        Poco::AutoPtr<Poco::XML::Text> speedRateReel = doc->createTextNode(std::to_string(reel.speedRate));
        speedRateReelXml->appendChild(speedRateReel);
        reelXml->appendChild(speedRateReelXml);
        
        refSync->appendChild(reelXml);
    }
    root->appendChild(refSync);

    Poco::AutoPtr<Poco::XML::Element> newCpl = doc->createElement("new");
    for (int i = 0; i < reelsCplNew.size(); i++) {
        Synchronizable::Reel reel = reelsCplNew.at(i);
        Poco::AutoPtr<Poco::XML::Element> reelXml = doc->createElement("reel");

        Poco::AutoPtr<Poco::XML::Element> idReelXml = doc->createElement("id");
        Poco::AutoPtr<Poco::XML::Text> idReel = doc->createTextNode(reel.id);
        idReelXml->appendChild(idReel);
        reelXml->appendChild(idReelXml);

        Poco::AutoPtr<Poco::XML::Element> durationReelXml = doc->createElement("duration");
        Poco::AutoPtr<Poco::XML::Text> durationReel = doc->createTextNode(std::to_string(reel.duration));
        durationReelXml->appendChild(durationReel);
        reelXml->appendChild(durationReelXml);

        Poco::AutoPtr<Poco::XML::Element> speedRateReelXml = doc->createElement("speedRate");
        Poco::AutoPtr<Poco::XML::Text> speedRateReel = doc->createTextNode(std::to_string(reel.speedRate));
        speedRateReelXml->appendChild(speedRateReel);
        reelXml->appendChild(speedRateReelXml);
        
        newCpl->appendChild(reelXml);
    }
    root->appendChild(newCpl);

    Poco::AutoPtr<Poco::XML::Element> matches = doc->createElement("matches");
    if (byId) {
        for (int i = 0; i < matchesById.size(); i++) {
            Synchro::Match2 match = matchesById.at(i);
            Poco::AutoPtr<Poco::XML::Element> matchXml = doc->createElement("match");

            Poco::AutoPtr<Poco::XML::Element> startRefXml = doc->createElement("startRef");
            Poco::AutoPtr<Poco::XML::Text> startRef = doc->createTextNode(std::to_string(match.startMsMatchRef));
            startRefXml->appendChild(startRef);
            matchXml->appendChild(startRefXml);

            Poco::AutoPtr<Poco::XML::Element> endRefXml = doc->createElement("endRef");
            Poco::AutoPtr<Poco::XML::Text> endRef = doc->createTextNode(std::to_string(match.endMsMatchRef));
            endRefXml->appendChild(endRef);
            matchXml->appendChild(endRefXml);

            Poco::AutoPtr<Poco::XML::Element> startNewXml = doc->createElement("startNew");
            Poco::AutoPtr<Poco::XML::Text> startNew = doc->createTextNode(std::to_string(match.startMsMatchNew));
            startNewXml->appendChild(startNew);
            matchXml->appendChild(startNewXml);

            Poco::AutoPtr<Poco::XML::Element> endNewXml = doc->createElement("endNew");
            Poco::AutoPtr<Poco::XML::Text> endNew = doc->createTextNode(std::to_string(match.endMsMatchNew));
            endNewXml->appendChild(endNew);
            matchXml->appendChild(endNewXml);
            
            matches->appendChild(matchXml);
        }
    }
    else {
        for (int i = 0; i < matchesByDuration.size(); i++) {
            Synchro::Match match = matchesByDuration.at(i);
            Poco::AutoPtr<Poco::XML::Element> matchXml = doc->createElement("match");

            Poco::AutoPtr<Poco::XML::Element> idxStartRefXml = doc->createElement("idxStartRef");
            Poco::AutoPtr<Poco::XML::Text> idxStartRef = doc->createTextNode(std::to_string(match.idxReelRef));
            idxStartRefXml->appendChild(idxStartRef);
            matchXml->appendChild(idxStartRefXml);

            Poco::AutoPtr<Poco::XML::Element> nbReelRefXml = doc->createElement("nbReelRef");
            Poco::AutoPtr<Poco::XML::Text> nbReelRef = doc->createTextNode(std::to_string(match.nbIdxReelRef));
            nbReelRefXml->appendChild(nbReelRef);
            matchXml->appendChild(nbReelRefXml);

            Poco::AutoPtr<Poco::XML::Element> idxStartNewXml = doc->createElement("idxStartNew");
            Poco::AutoPtr<Poco::XML::Text> idxStartNew = doc->createTextNode(std::to_string(match.idxReelNew));
            idxStartNewXml->appendChild(idxStartNew);
            matchXml->appendChild(idxStartNewXml);

            Poco::AutoPtr<Poco::XML::Element> idxEndNewXml = doc->createElement("idxEndNew");
            Poco::AutoPtr<Poco::XML::Text> idxEndNew = doc->createTextNode(std::to_string(match.nbIdxReelNew));
            idxEndNewXml->appendChild(idxEndNew);
            matchXml->appendChild(idxEndNewXml);
            
            matches->appendChild(matchXml);
        }
    }
    root->appendChild(matches);

    Poco::AutoPtr<Poco::XML::Element> inters = doc->createElement("interMatches");
    for (int i = 0; i < interMatches.size(); i++) {
        Synchro::InterMatch inter = interMatches.at(i);
        Poco::AutoPtr<Poco::XML::Element> interXml = doc->createElement("inter");

        Poco::AutoPtr<Poco::XML::Element> startRefXml = doc->createElement("startRef");
        Poco::AutoPtr<Poco::XML::Text> startRef = doc->createTextNode(std::to_string(inter.startMsMatchRef));
        startRefXml->appendChild(startRef);
        interXml->appendChild(startRefXml);

        Poco::AutoPtr<Poco::XML::Element> startRefUncutXml = doc->createElement("startRefUncut");
        Poco::AutoPtr<Poco::XML::Text> startRefUncut = doc->createTextNode(std::to_string(inter.startMsMatchRefUncut));
        startRefUncutXml->appendChild(startRefUncut);
        interXml->appendChild(startRefUncutXml);

        Poco::AutoPtr<Poco::XML::Element> endRefXml = doc->createElement("endRef");
        Poco::AutoPtr<Poco::XML::Text> endRef = doc->createTextNode(std::to_string(inter.endMsMatchRef));
        endRefXml->appendChild(endRef);
        interXml->appendChild(endRefXml);

        Poco::AutoPtr<Poco::XML::Element> endRefUncutXml = doc->createElement("endRefUncut");
        Poco::AutoPtr<Poco::XML::Text> endRefUncut = doc->createTextNode(std::to_string(inter.endMsMatchRefUncut));
        endRefUncutXml->appendChild(endRefUncut);
        interXml->appendChild(endRefUncutXml);

        Poco::AutoPtr<Poco::XML::Element> startNewXml = doc->createElement("startNew");
        Poco::AutoPtr<Poco::XML::Text> startNew = doc->createTextNode(std::to_string(inter.startMsMatchNew));
        startNewXml->appendChild(startNew);
        interXml->appendChild(startNewXml);

        Poco::AutoPtr<Poco::XML::Element> endNewXml = doc->createElement("endNew");
        Poco::AutoPtr<Poco::XML::Text> endNew = doc->createTextNode(std::to_string(inter.endMsMatchNew));
        endNewXml->appendChild(endNew);
        interXml->appendChild(endNewXml);

        Poco::AutoPtr<Poco::XML::Element> validXml = doc->createElement("valid");
        Poco::AutoPtr<Poco::XML::Text> valid = doc->createTextNode(std::to_string(inter.valid));
        validXml->appendChild(valid);
        interXml->appendChild(validXml);
        
        inters->appendChild(interXml);
    }
    root->appendChild(inters);

    Poco::AutoPtr<Poco::XML::Element> invalid = doc->createElement("invalidCuts");
    if (byId) {
        for (int i = 0; i < invalidCuts.size(); i++) {
            Synchro::InvalidCut cut = invalidCuts.at(i);
            Poco::AutoPtr<Poco::XML::Element> cutsXml = doc->createElement("inter");

            Poco::AutoPtr<Poco::XML::Element> startXml = doc->createElement("start");
            Poco::AutoPtr<Poco::XML::Text> start = doc->createTextNode(std::to_string(cut.startMs));
            startXml->appendChild(start);
            cutsXml->appendChild(startXml);

            Poco::AutoPtr<Poco::XML::Element> endXml = doc->createElement("end");
            Poco::AutoPtr<Poco::XML::Text> end = doc->createTextNode(std::to_string(cut.endMs));
            endXml->appendChild(end);
            cutsXml->appendChild(endXml);

            Poco::AutoPtr<Poco::XML::Element> leftXml = doc->createElement("left");
            Poco::AutoPtr<Poco::XML::Text> left = doc->createTextNode(std::to_string(cut.left));
            leftXml->appendChild(left);
            cutsXml->appendChild(leftXml);

            Poco::AutoPtr<Poco::XML::Element> rightXml = doc->createElement("right");
            Poco::AutoPtr<Poco::XML::Text> right = doc->createTextNode(std::to_string(cut.right));
            rightXml->appendChild(right);
            cutsXml->appendChild(rightXml);

            Poco::AutoPtr<Poco::XML::Element> outLeftXml = doc->createElement("outLeft");
            Poco::AutoPtr<Poco::XML::Text> outLeft = doc->createTextNode(std::to_string(cut.nbFrameOutLeft));
            outLeftXml->appendChild(outLeft);
            cutsXml->appendChild(outLeftXml);

            Poco::AutoPtr<Poco::XML::Element> outRightXml = doc->createElement("outRight");
            Poco::AutoPtr<Poco::XML::Text> outRight = doc->createTextNode(std::to_string(cut.nbFrameOutRight));
            outRightXml->appendChild(outRight);
            cutsXml->appendChild(outRightXml);

            Poco::AutoPtr<Poco::XML::Element> nbMissingXml = doc->createElement("nbMissing");
            Poco::AutoPtr<Poco::XML::Text> nbMissing = doc->createTextNode(std::to_string(cut.nbMissingFrame));
            nbMissingXml->appendChild(nbMissing);
            cutsXml->appendChild(nbMissingXml);

            invalid->appendChild(cutsXml);
        }
    }
    root->appendChild(invalid);
}

void RepositoryResponse::createDownloadSyncXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("Content");
    doc->appendChild(root);

    Poco::AutoPtr<Poco::XML::Text> contentText = doc->createTextNode(contentSync);
    root->appendChild(contentText);
}