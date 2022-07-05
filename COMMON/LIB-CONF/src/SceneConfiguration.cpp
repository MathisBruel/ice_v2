#include "SceneConfiguration.h"

SceneConfiguration::SceneConfiguration(std::string filename)
{
    pathFile = filename;
}
SceneConfiguration::~SceneConfiguration() 
{
    std::map<std::string, SceneConfiguration::Scene*>::iterator sceneIt;
    for (sceneIt = scenes.begin(); sceneIt != scenes.end(); sceneIt++) {
        std::map<std::string, SceneConfiguration::ColorData*>::iterator itBack;
        for (itBack = sceneIt->second->backlights.begin(); itBack != sceneIt->second->backlights.end(); itBack++) {
            delete itBack->second;
        }
        for (itBack = sceneIt->second->panels.begin(); itBack != sceneIt->second->panels.end(); itBack++) {
            delete itBack->second;
        }
        std::map<std::string, SceneConfiguration::ParDatas*>::iterator itPar;
        for (itPar = sceneIt->second->pars.begin(); itPar != sceneIt->second->pars.end(); itPar++) {
            delete itPar->second;
        }
        std::map<std::string, SceneConfiguration::MovingHeadDatas*>::iterator itMH;
        for (itMH = sceneIt->second->movingheads.begin(); itMH != sceneIt->second->movingheads.end(); itMH++) {
            delete itMH->second;
        }
        delete sceneIt->second;
    }
}

bool SceneConfiguration::load()
{
    // -- check file exists
    Poco::File fileScene(pathFile);
    if (!fileScene.exists()) {
        Poco::Logger::get("SceneConfiguration").error("Scene configuration file " + pathFile + " does not exists !", __FILE__, __LINE__);
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
        Poco::Logger::get("SceneConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
        in.close();
        return false;
    }
    in.close();

    Poco::XML::Node* configuration = nullptr;
    Poco::XML::NodeList* root = doc->childNodes();
    for (int h = 0; h < root->length(); h++) {
        if (root->item(h)->nodeName() == "configuration") {
            configuration = root->item(h);
        }
    }
    if (configuration == nullptr) {
        Poco::Logger::get("SceneConfiguration").error("No node configuration in SCENE configuration file !", __FILE__, __LINE__);
        root->release();
        doc->release();
        return false;
    }

    Poco::XML::NodeList* confList = configuration->childNodes();
    for (int i = 0; i < confList->length(); i++) {
        Poco::XML::Node* item = confList->item(i);
        if (item->nodeName() == "scene") {
            Scene* scene = loadScene(item);
            if (scene != nullptr && !scene->name.empty()) {
                scenes.insert_or_assign(scene->name, scene);
            }
            else {
                confList->release();
                root->release();
                doc->release();
                return false;
            }
        }
    }

    confList->release();
    root->release();
    doc->release();
    return true;
}

SceneConfiguration::Scene* SceneConfiguration::loadScene(Poco::XML::Node *sceneXml)
{
    Scene* scene = new Scene();
    Poco::XML::NodeList* listParam = sceneXml->childNodes();
    for (int j = 0; j < listParam->length(); j++) {
        Poco::XML::Node* paramScene = listParam->item(j);

        if (paramScene->nodeName() == "name") {
            scene->name = paramScene->innerText();
        }
        else if (paramScene->nodeName() == "panel") {

            ColorData* color = new ColorData();
            Poco::XML::NodeList* listParamPanel = paramScene->childNodes();
            for (int k = 0; k < listParamPanel->length(); k++) {
                Poco::XML::Node* paramPanel = listParamPanel->item(k);
                if (paramPanel->nodeName() == "red") {
                    color->red = std::stoi(paramPanel->innerText());
                }
                else if (paramPanel->nodeName() == "green") {
                    color->green = std::stoi(paramPanel->innerText());
                }
                else if (paramPanel->nodeName() == "blue") {
                    color->blue = std::stoi(paramPanel->innerText());
                }
                else if (paramPanel->nodeName() == "name") {
                    scene->panels.insert_or_assign(paramPanel->innerText(), color);
                }
            }
            listParamPanel->release();
        }

        else if (paramScene->nodeName() == "backlight") {

            ColorData* color = new ColorData();
            Poco::XML::NodeList* listParamBack = paramScene->childNodes();
            for (int k = 0; k < listParamBack->length(); k++) {
                Poco::XML::Node* paramBacklight = listParamBack->item(k);
                if (paramBacklight->nodeName() == "red") {
                    color->red = std::stoi(paramBacklight->innerText());
                }
                else if (paramBacklight->nodeName() == "green") {
                    color->green = std::stoi(paramBacklight->innerText());
                }
                else if (paramBacklight->nodeName() == "blue") {
                    color->blue = std::stoi(paramBacklight->innerText());
                }
                else if (paramBacklight->nodeName() == "name") {
                    scene->backlights.insert_or_assign(paramBacklight->innerText(), color);
                }
            }
            listParamBack->release();
        }

        else if (paramScene->nodeName() == "par") {

            ParDatas* pars = new ParDatas();
            Poco::XML::NodeList* listParamPar = paramScene->childNodes();
            for (int k = 0; k < listParamPar->length(); k++) {
                Poco::XML::Node* paramPars = listParamPar->item(k);
                if (paramPars->nodeName() == "red") {
                    pars->color.red = std::stoi(paramPars->innerText());
                }
                else if (paramPars->nodeName() == "green") {
                    pars->color.green = std::stoi(paramPars->innerText());
                }
                else if (paramPars->nodeName() == "blue") {
                    pars->color.blue = std::stoi(paramPars->innerText());
                }
                else if (paramPars->nodeName() == "white") {
                    pars->white = std::stoi(paramPars->innerText());
                }
                else if (paramPars->nodeName() == "zoom") {
                    pars->zoom = std::stoi(paramPars->innerText());
                }
                else if (paramPars->nodeName() == "intensity") {
                    pars->intensity = std::stoi(paramPars->innerText());
                }
                else if (paramPars->nodeName() == "name") {
                    scene->pars.insert_or_assign(paramPars->innerText(), pars);
                }
            }
            listParamPar->release();
        }

        else if (paramScene->nodeName() == "movinghead") {

            MovingHeadDatas* mh = new MovingHeadDatas();
            Poco::XML::NodeList* listParamMH = paramScene->childNodes();
            for (int k = 0; k < listParamMH->length(); k++) {
                Poco::XML::Node* paramMH = listParamMH->item(k);
                if (paramMH->nodeName() == "red") {
                    mh->color.red = std::stoi(paramMH->innerText());
                }
                else if (paramMH->nodeName() == "green") {
                    mh->color.green = std::stoi(paramMH->innerText());
                }
                else if (paramMH->nodeName() == "blue") {
                    mh->color.blue = std::stoi(paramMH->innerText());
                }
                else if (paramMH->nodeName() == "white") {
                    mh->white = std::stoi(paramMH->innerText());
                }
                else if (paramMH->nodeName() == "zoom") {
                    mh->zoom = std::stoi(paramMH->innerText());
                }
                else if (paramMH->nodeName() == "intensity") {
                    mh->intensity = std::stoi(paramMH->innerText());
                }
                else if (paramMH->nodeName() == "ratioX") {
                    mh->ratioX = std::stod(paramMH->innerText());
                }
                else if (paramMH->nodeName() == "ratioY") {
                    mh->ratioY = std::stod(paramMH->innerText());
                }
                else if (paramMH->nodeName() == "name") {
                    scene->movingheads.insert_or_assign(paramMH->innerText(), mh);
                }
            }
            listParamMH->release();
        }
    }
    listParam->release();

    return scene;
}

bool SceneConfiguration::save()
{
    std::ofstream m_OutFile;
    m_OutFile.open(pathFile.c_str(), std::ios::out);
    m_OutFile << getConfToStringXml();
    m_OutFile.close();

    return true;
}

std::string SceneConfiguration::getConfToStringXml()
{
    Poco::AutoPtr<Poco::XML::Document> doc = new Poco::XML::Document;
    Poco::AutoPtr<Poco::XML::Element> root = doc->createElement("configuration");
    doc->appendChild(root);

    std::map<std::string, SceneConfiguration::Scene *>::iterator itScene;
    for (itScene = scenes.begin(); itScene != scenes.end(); itScene++) {
        Poco::AutoPtr<Poco::XML::Element> scene = doc->createElement("scene");

        Poco::AutoPtr<Poco::XML::Element> nameScene = doc->createElement("name");
        Poco::AutoPtr<Poco::XML::Text> nameSceneStr = doc->createTextNode(itScene->first);
        nameScene->appendChild(nameSceneStr);
        scene->appendChild(nameScene);

        std::map<std::string, SceneConfiguration::ColorData *>::iterator itColor;
        for (itColor = itScene->second->panels.begin(); itColor != itScene->second->panels.end(); itColor++) {
            Poco::AutoPtr<Poco::XML::Element> panelScene = doc->createElement("panel");

            Poco::AutoPtr<Poco::XML::Element> panelName = doc->createElement("name");
            Poco::AutoPtr<Poco::XML::Text> panelNameStr = doc->createTextNode(itColor->first);
            panelName->appendChild(panelNameStr);
            panelScene->appendChild(panelName);

            Poco::AutoPtr<Poco::XML::Element> panelRed = doc->createElement("red");
            Poco::AutoPtr<Poco::XML::Text> panelRedStr = doc->createTextNode(std::to_string(itColor->second->red));
            panelRed->appendChild(panelRedStr);
            panelScene->appendChild(panelRed);

            Poco::AutoPtr<Poco::XML::Element> panelGreen = doc->createElement("green");
            Poco::AutoPtr<Poco::XML::Text> panelGreenStr = doc->createTextNode(std::to_string(itColor->second->green));
            panelGreen->appendChild(panelGreenStr);
            panelScene->appendChild(panelGreen);

            Poco::AutoPtr<Poco::XML::Element> panelBlue = doc->createElement("blue");
            Poco::AutoPtr<Poco::XML::Text> panelBlueStr = doc->createTextNode(std::to_string(itColor->second->blue));
            panelBlue->appendChild(panelBlueStr);
            panelScene->appendChild(panelBlue);

            scene->appendChild(panelScene);
        }

        for (itColor = itScene->second->backlights.begin(); itColor != itScene->second->backlights.end(); itColor++) {
            Poco::AutoPtr<Poco::XML::Element> backScene = doc->createElement("backlight");

            Poco::AutoPtr<Poco::XML::Element> backName = doc->createElement("name");
            Poco::AutoPtr<Poco::XML::Text> backNameStr = doc->createTextNode(itColor->first);
            backName->appendChild(backNameStr);
            backScene->appendChild(backName);

            Poco::AutoPtr<Poco::XML::Element> backRed = doc->createElement("red");
            Poco::AutoPtr<Poco::XML::Text> backRedStr = doc->createTextNode(std::to_string(itColor->second->red));
            backRed->appendChild(backRedStr);
            backScene->appendChild(backRed);

            Poco::AutoPtr<Poco::XML::Element> backGreen = doc->createElement("green");
            Poco::AutoPtr<Poco::XML::Text> backGreenStr = doc->createTextNode(std::to_string(itColor->second->green));
            backGreen->appendChild(backGreenStr);
            backScene->appendChild(backGreen);

            Poco::AutoPtr<Poco::XML::Element> backBlue = doc->createElement("blue");
            Poco::AutoPtr<Poco::XML::Text> backBlueStr = doc->createTextNode(std::to_string(itColor->second->blue));
            backBlue->appendChild(backBlueStr);
            backScene->appendChild(backBlue);

            scene->appendChild(backScene);
        }

        std::map<std::string, SceneConfiguration::ParDatas *>::iterator itPar;
        for (itPar = itScene->second->pars.begin(); itPar != itScene->second->pars.end(); itPar++) {
            Poco::AutoPtr<Poco::XML::Element> parScene = doc->createElement("par");

            Poco::AutoPtr<Poco::XML::Element> parName = doc->createElement("name");
            Poco::AutoPtr<Poco::XML::Text> parNameStr = doc->createTextNode(itPar->first);
            parName->appendChild(parNameStr);
            parScene->appendChild(parName);

            Poco::AutoPtr<Poco::XML::Element> parRed = doc->createElement("red");
            Poco::AutoPtr<Poco::XML::Text> parRedStr = doc->createTextNode(std::to_string(itPar->second->color.red));
            parRed->appendChild(parRedStr);
            parScene->appendChild(parRed);

            Poco::AutoPtr<Poco::XML::Element> parGreen = doc->createElement("green");
            Poco::AutoPtr<Poco::XML::Text> parGreenStr = doc->createTextNode(std::to_string(itPar->second->color.green));
            parGreen->appendChild(parGreenStr);
            parScene->appendChild(parGreen);

            Poco::AutoPtr<Poco::XML::Element> parBlue = doc->createElement("blue");
            Poco::AutoPtr<Poco::XML::Text> parBlueStr = doc->createTextNode(std::to_string(itPar->second->color.blue));
            parBlue->appendChild(parBlueStr);
            parScene->appendChild(parBlue);

            Poco::AutoPtr<Poco::XML::Element> parWhite = doc->createElement("white");
            Poco::AutoPtr<Poco::XML::Text> parWhiteStr = doc->createTextNode(std::to_string(itPar->second->white));
            parWhite->appendChild(parWhiteStr);
            parScene->appendChild(parWhite);

            Poco::AutoPtr<Poco::XML::Element> parIntensity = doc->createElement("intensity");
            Poco::AutoPtr<Poco::XML::Text> parIntensityStr = doc->createTextNode(std::to_string(itPar->second->intensity));
            parIntensity->appendChild(parIntensityStr);
            parScene->appendChild(parIntensity);

            Poco::AutoPtr<Poco::XML::Element> parZoom = doc->createElement("zoom");
            Poco::AutoPtr<Poco::XML::Text> parZoomStr = doc->createTextNode(std::to_string(itPar->second->zoom));
            parZoom->appendChild(parZoomStr);
            parScene->appendChild(parZoom);

            scene->appendChild(parScene);
        }

        root->appendChild(scene);

        std::map<std::string, SceneConfiguration::MovingHeadDatas *>::iterator itMh;
        for (itMh = itScene->second->movingheads.begin(); itMh != itScene->second->movingheads.end(); itMh++) {
            Poco::AutoPtr<Poco::XML::Element> mhScene = doc->createElement("movinghead");

            Poco::AutoPtr<Poco::XML::Element> mhName = doc->createElement("name");
            Poco::AutoPtr<Poco::XML::Text> mhNameStr = doc->createTextNode(itMh->first);
            mhName->appendChild(mhNameStr);
            mhScene->appendChild(mhName);

            Poco::AutoPtr<Poco::XML::Element> mhRed = doc->createElement("red");
            Poco::AutoPtr<Poco::XML::Text> mhRedStr = doc->createTextNode(std::to_string(itMh->second->color.red));
            mhRed->appendChild(mhRedStr);
            mhScene->appendChild(mhRed);

            Poco::AutoPtr<Poco::XML::Element> mhGreen = doc->createElement("green");
            Poco::AutoPtr<Poco::XML::Text> mhGreenStr = doc->createTextNode(std::to_string(itMh->second->color.green));
            mhGreen->appendChild(mhGreenStr);
            mhScene->appendChild(mhGreen);

            Poco::AutoPtr<Poco::XML::Element> mhBlue = doc->createElement("blue");
            Poco::AutoPtr<Poco::XML::Text> mhBlueStr = doc->createTextNode(std::to_string(itMh->second->color.blue));
            mhBlue->appendChild(mhBlueStr);
            mhScene->appendChild(mhBlue);

            Poco::AutoPtr<Poco::XML::Element> mhWhite = doc->createElement("white");
            Poco::AutoPtr<Poco::XML::Text> mhWhiteStr = doc->createTextNode(std::to_string(itMh->second->white));
            mhWhite->appendChild(mhWhiteStr);
            mhScene->appendChild(mhWhite);

            Poco::AutoPtr<Poco::XML::Element> mhIntensity = doc->createElement("intensity");
            Poco::AutoPtr<Poco::XML::Text> mhIntensityStr = doc->createTextNode(std::to_string(itMh->second->intensity));
            mhIntensity->appendChild(mhIntensityStr);
            mhScene->appendChild(mhIntensity);

            Poco::AutoPtr<Poco::XML::Element> mhZoom = doc->createElement("zoom");
            Poco::AutoPtr<Poco::XML::Text> mhZoomStr = doc->createTextNode(std::to_string(itMh->second->zoom));
            mhZoom->appendChild(mhZoomStr);
            mhScene->appendChild(mhZoom);

            Poco::AutoPtr<Poco::XML::Element> mhRatioX = doc->createElement("ratioX");
            Poco::AutoPtr<Poco::XML::Text> mhRatioXStr = doc->createTextNode(std::to_string(itMh->second->ratioX));
            mhRatioX->appendChild(mhRatioXStr);
            mhScene->appendChild(mhRatioX);

            Poco::AutoPtr<Poco::XML::Element> mhRatioY = doc->createElement("ratioY");
            Poco::AutoPtr<Poco::XML::Text> mhRatioYStr = doc->createTextNode(std::to_string(itMh->second->ratioY));
            mhRatioY->appendChild(mhRatioYStr);
            mhScene->appendChild(mhRatioY);

            scene->appendChild(mhScene);
        }
    }

    std::ostringstream outStr;
    Poco::XML::DOMWriter builder;
    builder.setNewLine("\n");
    builder.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
    builder.writeNode(outStr, doc);

    doc->release();

    return outStr.str();
}

bool SceneConfiguration::addOrChangeScene(std::string sceneName, std::string contentXml)
{
    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    try {
        doc = parser.parseString(contentXml);
    } catch (std::exception &e) {
        Poco::Logger::get("SceneConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
        return false;
    }

    Poco::XML::Node* sceneNode = nullptr;
    for (int h = 0; h < doc->childNodes()->length(); h++) {
        if (doc->childNodes()->item(h)->nodeName() == "content") {
            sceneNode = doc->childNodes()->item(h);
        }
    }
    if (sceneNode == nullptr) {
        Poco::Logger::get("SceneConfiguration").error("No node scene in set SCENE function !", __FILE__, __LINE__);
        doc->release();
        return false;
    }

    Scene* scene = loadScene(sceneNode);
    if (scene != nullptr && !scene->name.empty()) {

        std::map<std::string, SceneConfiguration::Scene*>::iterator sceneIt = scenes.find(sceneName);
        if (sceneIt != scenes.end()) {
            removeScene(sceneName);
        }
        scenes.insert_or_assign(scene->name, scene);
        doc->release();
    }
    else {
        doc->release();
        return false;
    }
    save();

    return true;
}
bool SceneConfiguration::removeScene(std::string scene)
{
    std::map<std::string, SceneConfiguration::Scene*>::iterator sceneIt = scenes.find(scene);
    if (sceneIt != scenes.end()) {
        
        std::map<std::string, SceneConfiguration::ColorData*>::iterator itBack;
        for (itBack = sceneIt->second->backlights.begin(); itBack != sceneIt->second->backlights.end(); itBack++) {
            delete itBack->second;
            itBack = sceneIt->second->backlights.erase(itBack);
            if (itBack == sceneIt->second->backlights.end()) {
                break;
            }
        }
        for (itBack = sceneIt->second->panels.begin(); itBack != sceneIt->second->panels.end(); itBack++) {
            delete itBack->second;
            itBack = sceneIt->second->panels.erase(itBack);
            if (itBack == sceneIt->second->panels.end()) {
                break;
            }
        }
        std::map<std::string, SceneConfiguration::ParDatas*>::iterator itPar;
        for (itPar = sceneIt->second->pars.begin(); itPar != sceneIt->second->pars.end(); itPar++) {
            delete itPar->second;
            itPar = sceneIt->second->pars.erase(itPar);
            if (itPar == sceneIt->second->pars.end()) {
                break;
            }
        }
        std::map<std::string, SceneConfiguration::MovingHeadDatas*>::iterator itMH;
        for (itMH = sceneIt->second->movingheads.begin(); itMH != sceneIt->second->movingheads.end(); itMH++) {
            delete itMH->second;
            itMH = sceneIt->second->movingheads.erase(itMH);
            if (itMH == sceneIt->second->movingheads.end()) {
                break;
            }
        }
        delete sceneIt->second;
        scenes.erase(sceneIt);
        save();

        return true;
    }

    return false;
}

SceneConfiguration::Scene* SceneConfiguration::getSceneByName(std::string sceneName)
{
    std::map<std::string, SceneConfiguration::Scene *>::iterator it = scenes.find(sceneName);
    if (it == scenes.end()) {
        return nullptr;
    }
    else {
        return it->second;
    }
}