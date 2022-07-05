#include "videoSyncer.h"

VideoSyncer::VideoSyncer()
{
    try {
        Poco::Logger::root().setChannel(new Poco::ConsoleChannel());
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);
}

VideoSyncer::~VideoSyncer()
{
    
}

void VideoSyncer::initialize(Application& self) {
    Application::initialize(self);
}
void VideoSyncer::reinitialize(Application& self) {
    Application::reinitialize(self);
}
void VideoSyncer::uninitialize()
{
    Poco::Util::Application::uninitialize();
}

// -- ae514d321ecb1d6b5b380131abfc78da023bcc83 : black image : useful ?

void VideoSyncer::fillHashes()
{
    std::map<std::string, std::vector<int>>::iterator it;

    // -- read ref file
    VideoHandler* refVideo = new VideoHandler();
    refVideo->openVideo(fileRef);

    // -- wait for start
    while (refVideo->getNbFrames() == -1) {Timer::crossUsleep(1000);}
    int nbFramesRef = refVideo->getNbFrames();

    for (int i = 0; i < nbFramesRef; i++) {
        
        if (refVideo->hasImageAtIndex(i)) {

            Image* in = refVideo->getImageAtIndex(i);
            refVideo->deleteImageAtIndex(i);
            std::string hash = Converter::calculateSha1Buffer(in->getData(), in->getSizeBuffer());
            mapFrameRef.insert_or_assign(i, hash);
            it = mapRef.find(hash);
            if (it == mapRef.end()) {
                std::vector<int> vect;
                vect.push_back(i);
                mapRef.insert_or_assign(hash, vect);
            }
            else {
                std::vector<int> vect = it->second;
                vect.push_back(i);
                mapRef.insert_or_assign(hash, vect);
            }
            delete in;
        }
        else {
            Timer::crossUsleep(1000);
        }
    }

    delete refVideo;

    // -- read new file
    VideoHandler* newVideo = new VideoHandler();
    newVideo->openVideo(fileNew);

    // -- wait for start
    while (newVideo->getNbFrames() == -1) {Timer::crossUsleep(1000);}
    int nbFramesNew = newVideo->getNbFrames();

    for (int i = 0; i < nbFramesNew; i++) {
        
        if (newVideo->hasImageAtIndex(i)) {

            Image* in = newVideo->getImageAtIndex(i);
            newVideo->deleteImageAtIndex(i);
            std::string hash = Converter::calculateSha1Buffer(in->getData(), in->getSizeBuffer());
            mapFrameNew.insert_or_assign(i, hash);
            it = mapNew.find(hash);
            if (it == mapNew.end()) {
                std::vector<int> vect;
                vect.push_back(i);
                mapNew.insert_or_assign(hash, vect);
            }
            else {
                std::vector<int> vect = it->second;
                vect.push_back(i);
                mapNew.insert_or_assign(hash, vect);
            }
            delete in;
        }
        else {
            Timer::crossUsleep(1000);
        }
    }

    delete refVideo;
}

void VideoSyncer::parseScene()
{
    // -- load file
    std::ifstream in(fileScene);
    Poco::XML::InputSource src(in);

    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parse(&src);
    } catch (std::exception &e) {
        Poco::Logger::get("LutConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
        in.close();
        return;
    }
    in.close();

    Poco::XML::Node* node = doc->getNodeByPath("xmeml/sequence/media/video/track[@MZ.TrackTargeted='1']");
    Poco::XML::NodeList* clips = node->childNodes();
    for (int h = 0; h < clips->length(); h++) {
        Poco::XML::Node* itemClip = clips->item(h);
        Poco::XML::NodeList* clipItems = itemClip->childNodes();
        for (int i = 0; i < clipItems->length(); i++) {
            if (clipItems->item(i)->nodeName() == "start") {
                int startScene = std::stoi(clipItems->item(i)->innerText());
                sceneCuts.insert_or_assign(startScene, 0);
            }
        }
        clipItems->release();
    }
    clips->release();
    doc->release();
}

void VideoSyncer::checkFeature()
{
    // --------------------------------------
    // FIRST PASS
    // check all frames from ref that appears only once
    // --------------------------------------
    std::map<int, FrameMatch*> resultFirst;
    std::map<std::string, std::vector<int>>::iterator itRef;
    std::map<std::string, std::vector<int>>::iterator itNew;
    for (itRef = mapRef.begin(); itRef != mapRef.end(); itRef++) {

        std::string hashRef = itRef->first;
        std::vector<int> frames = itRef->second;

        if (frames.size() == 1) {
    
            FrameMatch* match = new FrameMatch();
            itNew = mapNew.find(hashRef);
            match->frameRef = frames.at(0);
            match->sizeRef = 1;

            // -- missing
            if (itNew == mapNew.end()) {
                match->type = MISSING;
            }
            else {
                match->frameNew = itNew->second.at(0);
                match->sizeNew = 1;
                match->type = EQUAL;
            }
        }
    }

    // --------------------------------------
    // SECOND PASS
    // MERGE
    // --------------------------------------
    std::map<int, FrameMatch*> resultSecond;
    std::map<int, FrameMatch*>::iterator itFrame;
    int frameCurrent = -1;
    FrameMatch* currentMatch = nullptr;

    for (itFrame = resultFirst.begin(); itFrame != resultFirst.end(); itFrame++) {

        int frame = itFrame->first;
        FrameMatch* match = itFrame->second;

        // -- init or type has changed => new block
        if (frameCurrent == -1 || currentMatch->type != match->type) {
            currentMatch = new FrameMatch();
            currentMatch->frameRef = frame;
            currentMatch->sizeRef = 1;
            currentMatch->type = match->type;

            if (match->type == EQUAL) {
                currentMatch->frameNew = match->frameNew;
                currentMatch->sizeNew = 1;
            }

            frameCurrent = frame;
            resultSecond.insert_or_assign(currentMatch->frameRef, currentMatch);
        }

        // -- broken frame suite in ref
        else if (currentMatch->frameRef+currentMatch->sizeRef != frameCurrent) {

            currentMatch = new FrameMatch();
            currentMatch->frameRef = frame;
            currentMatch->sizeRef = 1;
            currentMatch->type = match->type;

            if (match->type == EQUAL) {
                currentMatch->frameNew = match->frameNew;
                currentMatch->sizeNew = 1;
            }

            frameCurrent = frame;
            resultSecond.insert_or_assign(currentMatch->frameRef, currentMatch);
        }
        // -- broken frame suite in new
        else if (currentMatch->type == EQUAL && currentMatch->frameNew+currentMatch->sizeNew != match->frameNew) {
            currentMatch = new FrameMatch();
            currentMatch->frameRef = frame;
            currentMatch->sizeRef = 1;
            currentMatch->type = match->type;
            currentMatch->frameNew = match->frameNew;
            currentMatch->sizeNew = 1;

            resultSecond.insert_or_assign(currentMatch->frameRef, currentMatch);
            frameCurrent = frame;
        }
        else {
            currentMatch->sizeRef++;
            if (currentMatch->type == EQUAL) {
                currentMatch->sizeNew++;
            }
        }
    }

    // --------------------------------------
    // THIRD PASS
    // Fill space with found in different spaces
    // --------------------------------------
    for (itRef = mapRef.begin(); itRef != mapRef.end(); itRef++) {

        std::string hashRef = itRef->first;
        std::vector<int> frames = itRef->second;

        if (frames.size() > 1) {
    
            int frameRef = -1;
            for (int i = 0; i < frames.size(); i++) {

                if (frameRef == -1) {
                    frameRef = i;
                }
            }
        }
    }
}

int VideoSyncer::main(const std::vector<std::string> &arguments)
{
    for (int i = 0; i < arguments.size(); i++) {

        // -- arguments format "key:value"
        int posSep = arguments.at(i).find(":", 0);
        if (posSep == std::string::npos) {
            std::cout << "Options must be formatted as follow : \"key:value\"" << std::endl;
            continue;
        }

        std::string key = arguments.at(i).substr(0, posSep);
        std::string value = arguments.at(i).substr(posSep+1);

        // -- path to config files
        if (key == "fileRef") {fileRef = value;}
        else if (key == "fileNew") {fileNew = value;}
        else if (key == "fileScene") {fileScene = value;}
        else if (key == "type") {type = value;}
        else {std::cout << "Option " + key + " not recognized !" << std::endl;}
    }

    fillHashes();

    if (type == "TLR") {
        parseScene();
    }

    // -- FEATURE type 
    // -- match all with all
    else if (type == "FEATURE") {
        checkFeature();
    }
}

