#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/Node.h"
#include "Poco/Logger.h"

#pragma once

class ComplexParam
{

public: 

    ComplexParam();
    ~ComplexParam();

    void load(Poco::XML::Node* param);
    std::string toXmlString();

private:

    std::map<std::string, std::string> attributes;
    std::vector<ComplexParam> childs;
};