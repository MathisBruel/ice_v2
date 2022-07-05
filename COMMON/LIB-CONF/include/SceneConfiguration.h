#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#pragma once

#include "Poco/File.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/Logger.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"

class SceneConfiguration
{

public:

    struct ColorData {
        int red;
        int green;
        int blue;
    };

    struct ParDatas {
        ColorData color;
        int white;
        int zoom;
        int intensity;
    };

    struct MovingHeadDatas {
        ColorData color;
        int white;
        int zoom;
        int intensity;
        double ratioX;
        double ratioY;
    };

    struct Scene {
        std::string name;
        std::map<std::string, ColorData*> panels;
        std::map<std::string, ColorData*> backlights;
        std::map<std::string, ParDatas*> pars;
        std::map<std::string, MovingHeadDatas*> movingheads;
    };

    SceneConfiguration(std::string filename);
    ~SceneConfiguration();

    bool load();
    bool save();

    bool addOrChangeScene(std::string sceneName, std::string contentXml);
    bool removeScene(std::string scene);

    Scene* getSceneByName(std::string sceneName);

    std::string getConfToStringXml(); 

private:

    Scene* loadScene(Poco::XML::Node *sceneXml);

    std::string pathFile;
    std::map<std::string, Scene*> scenes;
};