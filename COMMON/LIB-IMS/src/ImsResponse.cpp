#include "ImsResponse.h"

ImsResponse::ImsResponse(std::string uuidFromCommand, Command::CommandType type)
    : CommandResponse(uuidFromCommand, type)
{

}
    
ImsResponse::~ImsResponse() 
{
    statusPlayer.reset();
    std::map<std::string, std::shared_ptr<CplInfos>>::iterator itList;
    for (itList = listOfCpls.begin() ; itList != listOfCpls.end() ; itList++) {
        itList->second.reset();
    }
    listOfCpls.clear();
}

void ImsResponse::fillDatas()
{
    if (status == CommandResponse::OK) {

        Poco::AutoPtr<Poco::XML::Document> doc = new Poco::XML::Document;

        if (type == Command::CommandType::GET_CPL_CONTENT) {
            createCplContentXml(doc);
        }
        else if (type == Command::CommandType::GET_CPL_CONTENT_NAME) {
            createCplContentNameXml(doc);
        }

        // -- create string XML
        std::ostringstream outStr;
        Poco::XML::DOMWriter builder;
        builder.setNewLine("\n");
        builder.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
        builder.writeNode(outStr, doc);
        datas = outStr.str();

        doc->release();
    }
}

void ImsResponse::createCplContentXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("Content");
    doc->appendChild(root);

    Poco::AutoPtr<Poco::XML::Text> contentText = doc->createTextNode(contentFile);
    root->appendChild(contentText);
}

void ImsResponse::createCplContentNameXml(Poco::XML::Document* doc)
{
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("CPLS");
    doc->appendChild(root);

    std::map<std::string, std::shared_ptr<CplInfos>>::iterator it;
    for (it = listOfCpls.begin(); it != listOfCpls.end(); it++) {
        if (it->second != nullptr) {
            Poco::AutoPtr<Poco::XML::Element> cpl = doc->createElement("CPL");
            cpl->setAttribute("id", it->second->getCplId());

            // -- content
            Poco::AutoPtr<Poco::XML::Element> content = doc->createElement("Content");
            std::string encodedCpl = Converter::encodeBase64(it->second->getContentFile());
            Poco::AutoPtr<Poco::XML::Text> contentText = doc->createTextNode(encodedCpl);
            content->appendChild(contentText);
            cpl->appendChild(content);
        }
    }
}