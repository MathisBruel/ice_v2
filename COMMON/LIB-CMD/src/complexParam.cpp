#include "complexParam.h"

ComplexParam::ComplexParam()
{

}
ComplexParam::~ComplexParam()
{

}

void ComplexParam::load(Poco::XML::Node* param)
{
    Poco::XML::NodeList * childsTemp = param->childNodes();
    Poco::XML::NamedNodeMap *attributesTemp = param->attributes();

    // -- set node name
    attributes.insert_or_assign("node", param->nodeName());

    // -- load all attributes
    for (int i = 0; i < attributesTemp->length(); i++) {
        attributes.insert_or_assign(attributesTemp->item(i)->nodeName(), attributesTemp->item(i)->innerText());
    }

    // -- set inner text if no childs
    if (childsTemp->length() == 1 && childsTemp->item(0)->nodeType() == 3) {
        attributes.insert_or_assign("inner", param->innerText());
    }

    // -- load childs
    else {
        for (int i = 0; i < childsTemp->length(); i++) {
            ComplexParam paramC;
            paramC.load(childsTemp->item(i));
            childs.push_back(paramC);
        }
    }
    attributesTemp->release();
    childsTemp->release();
}

std::string ComplexParam::toXmlString()
{
    std::string nodeName = attributes.find("node")->second;
    std::string toReturn = "<" + nodeName;

    std::map<std::string, std::string>::iterator itAtt;
    for (itAtt = attributes.begin(); itAtt != attributes.end(); itAtt++) {
        if (itAtt->first != "inner" && itAtt->first != "node") {
            toReturn += " " + itAtt->first + "=\"" + itAtt->second + "\"";
        }
    }
    toReturn += ">";

    itAtt = attributes.find("inner");
    if (itAtt != attributes.end()) {
        toReturn += itAtt->second;
    }
    else {
        for (int i = 0; i < childs.size(); i++) {
            toReturn += childs.at(i).toXmlString();
        }
    }

    toReturn += "</" + nodeName + ">";
    return toReturn;
}