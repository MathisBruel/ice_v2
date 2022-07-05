#include "cplFile.h"

CplFile::CplFile()
    : Synchronizable("")
{
    type = UnknownType;
    speedRate = -1.0;
}

CplFile::CplFile(std::string pathFile)
    : Synchronizable(pathFile)
{
    type = UnknownType;
    speedRate = -1.0;
}
CplFile::~CplFile() {}

bool CplFile::load()
{
    // -- check file exists
    Poco::File fileCpl(pathFile);
    if (!fileCpl.exists()) {
        Poco::Logger::get("CPL").error("CPL file " + pathFile + " does not exists !", __FILE__, __LINE__);
        return false;
    }

    // -- load file
    std::ifstream in(pathFile);
    Poco::XML::InputSource src(in);

    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;

    try {
        doc = parser.parse(&src);
    } catch (std::exception &e) {
        Poco::Logger::get("CPL").error("Exception when parsing XML CPL !", __FILE__, __LINE__);
        in.close();
        return false;
    }
    in.close();

    // -- parse and release
    bool toReturn = parseXmlCpl(doc);
    doc->release();
    return toReturn;
}
bool CplFile::loadCplFromContentString(std::string content)
{
    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parseString(content);
    } catch (std::exception &e) {
        Poco::Logger::get("CPL").error("Exception when parsing XML CPL !", __FILE__, __LINE__);
        Poco::Logger::get("CPL").error(content, __FILE__, __LINE__);
        return false;
    }

    bool toReturn = parseXmlCpl(doc);
    doc->release();
    return toReturn;
}

bool CplFile::parseXmlCpl(Poco::XML::Document* doc)
{
    // -- get and check existance of CPL tag
    Poco::XML::Node* CplTag = nullptr;
    Poco::XML::NodeList* childs = doc->childNodes();
    for (int h = 0; h < childs->length(); h++) {
        if (childs->item(h)->nodeName() == "CompositionPlaylist") {
            CplTag = childs->item(h);
        }
    }
    childs->release();
    if (CplTag == nullptr) {
        Poco::Logger::get("CPL").error("No node CompositionPlaylist in CPL file !", __FILE__, __LINE__);
        return false;
    }

    // -- parse parameters
    Poco::XML::NodeList* childs2 = CplTag->childNodes();
    for (int i = 0; i < childs2->length(); i++) {
        Poco::XML::Node* tagCpl = childs2->item(i);

        if (tagCpl->nodeName() == "Id") {
            std::string idNotParsed = tagCpl->innerText();
            int startIdPos = idNotParsed.find_last_of(":", std::string::npos);

            if (startIdPos == std::string::npos) {
                Poco::Logger::get("CPL").error("Could not parse id string of CPL !", __FILE__, __LINE__);
                return false;
            }

            cplId = idNotParsed.substr(startIdPos+1);
        }
        else if (tagCpl->nodeName() == "AnnotationText") {
            annotation = tagCpl->innerText();
        }
        else if (tagCpl->nodeName() == "ContentTitleText") {
            cplTitle = tagCpl->innerText();
        }
        else if (tagCpl->nodeName() == "ContentKind") {
            type = parseCplTypeFromString(tagCpl->innerText());
        }
        else if (tagCpl->nodeName() == "ReelList") {
            
            Poco::XML::NodeList* childs3 = tagCpl->childNodes();
            for (int j = 0; j < childs3->length(); j++) {
                Poco::XML::Node* tagReel = childs3->item(j);

                if (tagReel->nodeName() == "Reel") {

                    Reel reel;
                    Poco::XML::NodeList* childs4 = tagReel->childNodes();
                    for (int k = 0; k < childs4->length(); k++) {

                        Poco::XML::Node* paramReel = childs4->item(k);
                        if (paramReel->nodeName() == "AssetList") {

                            Poco::XML::NodeList* childs5 = paramReel->childNodes();
                            for (int l = 0; l < childs5->length(); l++) {
                                Poco::XML::Node* asset = childs5->item(l);

                                if (asset->nodeName() == "MainPicture" || asset->nodeName() == "sicpl:MainStereoscopicPicture" || 
                                    asset->nodeName() == "ns1:MainStereoscopicPicture" || asset->nodeName() == "msp-cpl:MainStereoscopicPicture") {
                                    
                                    Poco::XML::NodeList* childs6 = asset->childNodes();
                                    for (int m = 0; m < childs6->length(); m++) {
                                        Poco::XML::Node* paramPicture = childs6->item(m);

                                        if (paramPicture->nodeName() == "Id") {
                                            std::string idNotParsed = paramPicture->innerText();
                                            int startIdPos = idNotParsed.find_last_of(":", std::string::npos);

                                            if (startIdPos == std::string::npos) {
                                                Poco::Logger::get("CPL").error("Could not parse id string of Reel !", __FILE__, __LINE__);
                                                return false;
                                            }

                                            reel.id = idNotParsed.substr(startIdPos+1);
                                        }
                                        else if (paramPicture->nodeName() == "EditRate") {
                                            int posSep = paramPicture->innerText().find(" " , 0);
                                            reel.editRate = paramPicture->innerText();

                                            if (posSep == std::string::npos) {
                                                Poco::Logger::get("CPL").error("Could not parse edit rate !", __FILE__, __LINE__);
                                                return false;
                                            }
                                            int num = std::stoi(paramPicture->innerText().substr(0, posSep));
                                            int den = std::stoi(paramPicture->innerText().substr(posSep+1));
                                            reel.speedRate = (double)num / (double) den;
                                        }

                                        else if (paramPicture->nodeName() == "IntrinsicDuration") {
                                            reel.intrinsicDuration = stoi(paramPicture->innerText());
                                        }
                                        else if (paramPicture->nodeName() == "EntryPoint") {
                                            reel.entryPoint = stoi(paramPicture->innerText());
                                        }
                                        else if (paramPicture->nodeName() == "Duration") {
                                            reel.duration = stoi(paramPicture->innerText());
                                        }
                                        else if (paramPicture->nodeName() == "KeyId") {

                                            std::string idNotParsed = paramPicture->innerText();
                                            int startIdPos = idNotParsed.find_last_of(":", std::string::npos);

                                            if (startIdPos == std::string::npos) {
                                                Poco::Logger::get("CPL").error("Could not parse id string of Reel !", __FILE__, __LINE__);
                                                return false;
                                            }
                                            reel.keyId = idNotParsed.substr(startIdPos+1);
                                        }
                                        else if (paramPicture->nodeName() == "Hash") {
                                            reel.hash = paramPicture->innerText();
                                        }
                                    
                                    }
                                    childs6->release();
                                }
                            }
                            childs5->release();
                        }
                    }
                    childs4->release();
                    reels.push_back(reel);
                }
            }
            childs3->release();
        }
    }
    childs2->release();
    return true;
}

CplFile::CplType CplFile::parseCplTypeFromString(std::string type)
{
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);

    if (type == "feature" || type == "Feature") {
        return Feature;
    }
    else if (type == "trailer" || type == "Trailer") {
        return Trailer;
    }
    else if (type == "test") {
        return Test;
    }
    else if (type == "teaser") {
        return Teaser;
    }
    else if (type == "rating") {
        return Rating;
    }
    else if (type == "advertisement") {
        return Advertisement;
    }
    else if (type == "short") {
        return Short;
    }
    else if (type == "transitional") {
        return Transitional;
    }
    else if (type == "psa") {
        return PSA;
    }
    else if (type == "plicy") {
        return Policy;
    }
    else if (type == "live_cpl") {
        return Live_CPL;
    }
    else {
        return UnknownType;
    }   
}

CplFile::CplProperty CplFile::parseCplPropertyFromString(std::string property)
{
    std::transform(property.begin(), property.end(), property.begin(), ::tolower);

    if (property == "hfr") {
        return HFR;
    }
    else if (property == "hbr") {
        return HBR;
    }
    else if (property == "caption") {
        return Caption;
    }
    else if (property == "projectordata") {
        return ProjectorData;
    }
    else if (property == "subtitle") {
        return Subtitle;
    }
    else if (property == "atmos") {
        return Atmos;
    }
    else return UnknowProperty;
}

std::string CplFile::cplTypeToString(CplFile::CplType type)
{
    if (type == Feature) {
        return "Feature";
    }
    else if (type == Trailer) {
        return "Trailer";
    }
    else if (type == Test) {
        return "Test";
    }
    else if (type == Teaser) {
        return "Teaser";
    }
    else if (type == Rating) {
        return "Rating";
    }
    else if (type == Advertisement) {
        return "Advertisement";
    }
    else if (type == Short) {
        return "Short";
    }
    else if (type == Transitional) {
        return "Transitional";
    }
    else if (type == PSA) {
        return "PSA";
    }
    else if (type == Policy) {
        return "Policy";
    }
    else if (type == Live_CPL) {
        return "Live CPL";
    }
    else {
        return "Unknown";
    }  
}

std::string CplFile::cplPropertyToString(CplFile::CplProperty property)
{
    if (property == HFR) {
        return "HFR";
    }
    else if (property == HBR) {
        return "HBR";
    }
    else if (property == Caption) {
        return "Caption";
    }
    else if (property == ProjectorData) {
        return "ProjectorData";
    }
    else if (property == Subtitle) {
        return "Subtitle";
    }
    else if (property == Atmos) {
        return "Atmos";
    }
    else return "Unknow";
}

std::string CplFile::toString()
{
    std::string toReturn = "CPL : \n";
    toReturn += "Id : " + cplId + "\n";
    toReturn += "Annotation : " + annotation + "\n";
    toReturn += "Title : " + cplTitle + "\n";
    toReturn += "Type : " + cplTypeToString(type) + "\n";
    toReturn += "Reels : \n";
    std::vector<Reel>::iterator it;
    for (it = reels.begin(); it != reels.end(); it++) {
        toReturn += "Id : " + it->id + "\n";
        toReturn += "Duration : " + std::to_string(it->duration) + "\n";
        toReturn += "EditRate : " + std::to_string(it->speedRate) + "\n";
        toReturn += "EntryPoint : " + std::to_string(it->entryPoint) + "\n";
        toReturn += "InbtrinsicDuration : " + std::to_string(it->intrinsicDuration) + "\n";
        toReturn += "KeyId : " + it->keyId + "\n";
        toReturn += "Hash : " + it->hash + "\n";
        toReturn += "-----------------------------------------------\n";
    }
    return toReturn;
}