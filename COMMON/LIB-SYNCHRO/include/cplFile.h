#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>

#pragma once

#include "Poco/File.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "cutscenes.h"
#include "Poco/Logger.h"
#include "synchronizable.h"

class CplFile : public Synchronizable
{

public:

    enum CplType {
        UnknownType,
        Feature,
        Trailer,
        Test,
        Teaser,
        Rating,
        Advertisement,
        Short,
        Transitional,
        PSA,
        Policy,
        Live_CPL
    };

    enum CplProperty {
        HFR, // -- high frame rate
        HBR, 
        Caption,
        Subtitle,
        ProjectorData,
        Atmos,
        UnknowProperty
    };

    CplFile();
    CplFile(std::string pathFile);
    ~CplFile();

    bool load();
    bool loadCplFromContentString(std::string content);

    std::string getAnnotation() {return annotation;}
    CplType getTypeContent() {return type;}

    static CplType parseCplTypeFromString(std::string type);
    static CplProperty parseCplPropertyFromString(std::string property);
    static std::string cplTypeToString(CplType property);
    static std::string cplPropertyToString(CplProperty property);

    std::string toString();

private:

    bool parseXmlCpl(Poco::XML::Document* doc);
    std::string annotation;
    CplType type;
};