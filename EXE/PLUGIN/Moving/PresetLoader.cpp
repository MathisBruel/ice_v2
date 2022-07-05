#include "PresetLoader.h"

PresetLoader::PresetLoader(std::string inDirectory)
{
    directory = inDirectory;
}
PresetLoader::~PresetLoader()
{
    std::map<std::string, MovingHandler*>::iterator it;
    for (it = presetsPosition.begin(); it != presetsPosition.end(); it++) {
        delete it->second;
    }
    presetsPosition.clear();
    for (it = presetsColor.begin(); it != presetsColor.end(); it++) {
        delete it->second;
    }
    presetsColor.clear();
}

void PresetLoader::loadPresets()
{
    errorLoading = "";

    for (const auto & entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream file(entry.path());
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        int offsetExtension = entry.path().string().find(".", 0);
        if (offsetExtension != std::string::npos) {
            std::string extension = entry.path().string().substr(offsetExtension+1);
            MovingHandler* handler = new MovingHandler();

            if (extension.find("pos") != std::string::npos) {
                if (!handler->loadFromString(buffer.str())) {
                    errorLoading += entry.path().string() + "\n";
                }
                else {
                    presetsPosition.insert_or_assign(entry.path().string(), handler);
                }
            }
            else if (extension.find("color") != std::string::npos) {
                if (!handler->loadFromString(buffer.str())) {
                    errorLoading += entry.path().string() + "\n";
                }
                else {
                    presetsColor.insert_or_assign(entry.path().string(), handler);
                }
            }
        }
    }
}
std::string PresetLoader::savePresets(MovingHandler* handler, bool isColor)
{
    int num1 = std::rand() % 99;
    int num2 = std::rand() % 99;
    int num3 = std::rand() % 99;
    int num4 = std::rand() % 99;

    std::string filename = directory + "/" + std::to_string(num1) + "-" + std::to_string(num2) + "-" + 
                            std::to_string(num3) + "-" + std::to_string(num4) + "." + (isColor ? "color" : "pos");

    std::ofstream presetFile;
    presetFile.open(filename, std::ios::out | std::ios::trunc);
    presetFile << handler->getContentString(isColor);
    presetFile.close();

    return filename;
}

MovingHandler* PresetLoader::getHandlerPositionWithIndex(int index)
{
    std::map<std::string, MovingHandler *>::iterator it;
    int idx = 1;
    for (it = presetsPosition.begin(); it != presetsPosition.end(); it++) {

        if (idx == index) {
            return it->second;
        }

        idx++;
    }

    return nullptr;
}
MovingHandler* PresetLoader::getHandlerColorWithIndex(int index)
{
    std::map<std::string, MovingHandler *>::iterator it;
    int idx = 1;
    for (it = presetsColor.begin(); it != presetsColor.end(); it++) {

        if (idx == index) {
            return it->second;
        }

        idx++;
    }

    return nullptr;
}

std::string PresetLoader::getPositionPresetsString()
{
    std::string returnStr = "";
    int idx = 0;
    std::map<std::string, MovingHandler *>::iterator it;
    for (it = presetsPosition.begin(); it != presetsPosition.end(); it++) {
        
        std::string pathFile = it->first;
        std::string delim = "/\\";
        int offsetFilename = pathFile.find_last_of(delim);
        std::string filename = pathFile.substr(offsetFilename+1);
        int offsetExtension = filename.find_last_of(".");
        std::string name = filename.substr(0, offsetExtension);

        returnStr += name;
        if (idx < presetsPosition.size() - 1) {returnStr += " | ";}
        idx++;
    }
    return returnStr;
}
std::string PresetLoader::getColorPresetsString()
{
    std::string returnStr = "";
    int idx = 0;
    std::map<std::string, MovingHandler *>::iterator it;
    for (it = presetsColor.begin(); it != presetsColor.end(); it++) {
        
        std::string pathFile = it->first;
        std::string delim = "/\\";
        int offsetFilename = pathFile.find_last_of(delim);
        std::string filename = pathFile.substr(offsetFilename+1);
        int offsetExtension = filename.find_last_of(".");
        std::string name = filename.substr(0, offsetExtension);

        returnStr += name;
        if (idx < presetsColor.size() - 1) {returnStr += " | ";}
        idx++;
    }
    return returnStr;
}